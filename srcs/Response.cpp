#include "Response.hpp"
#include "WebServ.h"

Response::Response()
: _statusCode(200), _statusMessage("OK"), _contentType("text/html") {}

void Response::setStatus(int code, const std::string& message)
{
    _statusCode = code;
    _statusMessage = message;
}

void Response::setBody(const std::string& body)
{
    _body = body;
}

void Response::setContentType(const std::string& type)
{
    _contentType = type;
}

void Response::addHeader(const std::string& key, const std::string& value)
{
    _headers[key] = value;
}

/* ================= BUILD ================= */

std::string Response::build() const
{
    std::stringstream ss;

    ss << "HTTP/1.1 " << _statusCode << " " << _statusMessage << "\r\n";
    ss << "Content-Length: " << _body.length() << "\r\n";
    ss << "Content-Type: " << _contentType << "\r\n";

    for (std::map<std::string,std::string>::const_iterator it = _headers.begin();
         it != _headers.end(); ++it)
        ss << it->first << ": " << it->second << "\r\n";

    ss << "Connection: close\r\n\r\n";
    ss << _body;

    return ss.str();
}

/* ================= ENTRY ================= */

std::string Response::buildResponse(const RequestConfig& req,
                                    const ServerConfig& server)
{
    std::cout << "BUILD RESPONSE\n";
    std::cout << "URI: [" << req.uri << "]" << std::endl;
    const LocationConfig* loc = matchLocation(req.uri, server);

    if (!loc)
        return buildError(404);
    
	// REDIRECT
    if (!loc->redirect.empty())
    {
        std::map<int,std::string>::const_iterator it = loc->redirect.begin();
        return buildRedirect(it->first, it->second);
    }

    // METHOD CHECK
    bool allowed = false;
    for (size_t i = 0; i < loc->methods.size(); i++)
    {
        if (loc->methods[i] == req.method)
            allowed = true;
    }

    if (!allowed)
        return buildError(405);


    // CGI
    if (!loc->cgi.empty())
    {
        std::string ext = getExtension(req.uri);

        std::map<std::string,std::string>::const_iterator it =
            loc->cgi.find(ext);

        if (it != loc->cgi.end())
            return executeCgi(req, it->second);
    }

    // DISPATCH METHOD
    if (req.method == "GET")
        return handleGet(req, *loc, server);
    if (req.method == "POST")
        return handlePost(req, *loc);
    if (req.method == "DELETE")
        return handleDelete(req, *loc);

    return buildError(405);
}


/* ================= GET ================= */
std::string Response::handleGet(const RequestConfig& req,
                                const LocationConfig& loc,
                                const ServerConfig& server)
{
    Response res;

    std::string base = loc.root.empty() ? server.root : loc.root;

	std::string relative;

	if (loc.path == "/")
	{
		relative = req.uri;
	}
	else if (req.uri.find(loc.path) == 0)
	{
		relative = req.uri.substr(loc.path.length());
	}
	else
	{
		relative = req.uri;
	}

	if (relative.empty() || relative[0] != '/')
		relative = "/" + relative;

    std::string path = base + relative;
	std::cout << "base: [" << base << "]" << std::endl;
	std::cout << "relative: [" << relative << "]" << std::endl;
	std::cout << "TRY OPEN: [" << path << "]" << std::endl;

    struct stat s;
    if (stat(path.c_str(), &s) == 0 && S_ISDIR(s.st_mode))
    {
		std::string indexFile;

		if (!loc.index.empty())
			indexFile = path + "/" + loc.index;
		else
			indexFile = path + "/index.html";

		if (fileExists(indexFile))
			return buildFileResponse(indexFile);

		if (loc.autoindex)
		{
			std::cout << "generateautoindex" << std::endl;
			return generateAutoIndex(path, req.uri);
		}
		return buildError(403);
    }

	std::cout << "TRY OPEN: [" << path << "]" << std::endl;

    if (!fileExists(path))
        return buildError(404);

    std::string body = readFile(path);

    res.setBody(body);
    res.setContentType(getContentType(path));

    return res.build();
}

/* ================= POST ================= */

std::string Response::handlePost(const RequestConfig& req,
                                 const LocationConfig& loc)
{
    Response res;

    std::string path;

    if (!loc.upload_path.empty())
        path = loc.upload_path + "/upload.txt";
    else
        path = "./www/uploads/upload.txt";

    if (!writeFile(path, req.body))
        return buildError(500);

    std::stringstream body;
    body << "<h1>Upload OK</h1>";
    body << "<pre>" << req.body << "</pre>";

    res.setStatus(201, "Created");
    res.setBody(body.str());

    return res.build();
}

/* ================= DELETE ================= */

std::string Response::handleDelete(const RequestConfig& req,
                                   const LocationConfig& loc)
{
	(void)loc;
    Response res;

    std::string path = "./www" + req.uri;

    if (!fileExists(path))
        return buildError(404);

    if (!deleteFile(path))
        return buildError(500);

    res.setBody("<h1>Deleted</h1>");

    return res.build();
}

/* ================= CGI ================= */

std::string Response::executeCgi(const RequestConfig& req, const std::string& interpreter)
{
    int pipefd[2];
    if (pipe(pipefd) == -1)
        return buildError(500);

    pid_t pid = fork();
    if (pid < 0)
        return buildError(500);

    if (pid == 0)
    {
        // ===== CHILD =====

        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);

        std::string scriptPath = "./www" + req.uri;


        // ===== ENV =====
        std::string method = "REQUEST_METHOD=" + req.method;

        std::string contentLength = "CONTENT_LENGTH=0";
        std::map<std::string,std::string>::const_iterator it =
            req.headers.find("content-length");

        if (it != req.headers.end())
            contentLength = "CONTENT_LENGTH=" + it->second;

        std::string contentType = "CONTENT_TYPE=text/plain";
        it = req.headers.find("content-type");
        if (it != req.headers.end())
            contentType = "CONTENT_TYPE=" + it->second;

        char *envp[4];
        envp[0] = (char*)method.c_str();
        envp[1] = (char*)contentLength.c_str();
        envp[2] = (char*)contentType.c_str();
        envp[3] = NULL;

        char *argv[3];
        argv[0] = (char*)interpreter.c_str();
        argv[1] = (char*)scriptPath.c_str();
        argv[2] = NULL;

        execve(interpreter.c_str(), argv, envp);
        exit(1);
    }

    // ===== PARENT =====

    close(pipefd[1]);

    char buffer[4096];
    std::stringstream output;
    ssize_t bytes;

    while ((bytes = read(pipefd[0], buffer, sizeof(buffer))) > 0)
        output.write(buffer, bytes);

    close(pipefd[0]);
    waitpid(pid, NULL, 0);

    std::string raw = output.str();

    size_t pos = raw.find("\r\n\r\n");
    size_t offset = 4;

    if (pos == std::string::npos)
    {
        pos = raw.find("\n\n");
        offset = 2;
    }

    std::string body;
    std::string headers;

    if (pos != std::string::npos)
    {
        headers = raw.substr(0, pos);
        body = raw.substr(pos + offset);
    }
    else
    {
        body = raw;
    }

    Response res;
    res.setBody(body);

    res.setContentType("text/html");

    size_t hpos = headers.find("Content-Type:");
    if (hpos != std::string::npos)
    {
        size_t end = headers.find("\n", hpos);
        std::string line = headers.substr(hpos, end - hpos);

        size_t sep = line.find(":");
        if (sep != std::string::npos)
        {
            std::string type = line.substr(sep + 1);
            while (!type.empty() && type[0] == ' ')
                type.erase(0, 1);

            res.setContentType(type);
        }
    }

    return res.build();
}

/* ================= ERROR ================= */

std::string Response::buildError(int code)
{
    Response res;

    std::stringstream path;
    path << "./www/errors/" << code << ".html";

    std::string body;

    if (fileExists(path.str()))
        body = readFile(path.str());
    else
    {
        std::stringstream fallback;
        fallback << "<h1>" << code << " " << getStatusMessage(code) << "</h1>";
        body = fallback.str();
    }

    res.setStatus(code, getStatusMessage(code));
    res.setBody(body);

    return res.build();
}

/* ================= UTILS ================= */

bool Response::fileExists(const std::string& path)
{
    struct stat s;
    return stat(path.c_str(), &s) == 0;
}

bool Response::deleteFile(const std::string& path)
{
    return remove(path.c_str()) == 0;
}

std::string Response::readFile(const std::string& path)
{
    std::ifstream file(path.c_str());
    if (!file.is_open())
        return "";

    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

bool Response::writeFile(const std::string& path, const std::string& content)
{
    std::ofstream file(path.c_str());
    if (!file.is_open())
        return false;

    file << content;
    return true;
}

/* ================= MIME ================= */

std::string Response::getContentType(const std::string& path)
{
    if (path.find(".html") != std::string::npos) return "text/html";
    if (path.find(".css") != std::string::npos) return "text/css";
    if (path.find(".js") != std::string::npos) return "application/javascript";
    if (path.find(".png") != std::string::npos) return "image/png";
    if (path.find(".jpg") != std::string::npos) return "image/jpeg";
    return "text/plain";
}

/* ================= STATUS ================= */

std::string Response::getStatusMessage(int code)
{
    if (code == 200) return "OK";
    if (code == 201) return "Created";
    if (code == 204) return "No Content";
    if (code == 400) return "Bad Request";
    if (code == 404) return "Not Found";
    if (code == 405) return "Method Not Allowed";
    if (code == 500) return "Internal Server Error";

    return "Error";
}

/* ================= COOKIE ================= */

std::string Response::generateSessionId()
{
    std::stringstream ss;
    ss << rand();
    return ss.str();
}

const LocationConfig* Response::matchLocation(const std::string& uri,
                                              const ServerConfig& server)
{
    size_t maxLen = 0;
    const LocationConfig* best = NULL;

    for (size_t i = 0; i < server.locations.size(); i++)
    {
        const LocationConfig& loc = server.locations[i];

        bool match = false;

        if (loc.path == "/")
        {
            match = true;
        }
        else if (uri == loc.path)
        {
            match = true;
        }
        else if (uri.find(loc.path + "/") == 0)
        {
            match = true;
        }

        if (match && loc.path.length() > maxLen)
        {
            maxLen = loc.path.length();
            best = &loc;
        }
    }

    return best;
}

std::string Response::buildRedirect(int code, const std::string& url)
{
    std::stringstream ss;

    ss << "HTTP/1.1 " << code << " Moved Permanently\r\n";
    ss << "Location: " << url << "\r\n";
    ss << "Content-Length: 0\r\n";
    ss << "Connection: close\r\n\r\n";

    return ss.str();
}

std::string Response::getExtension(const std::string& path)
{
    size_t dot = path.rfind('.');
    if (dot == std::string::npos)
        return "";
    return path.substr(dot);
}

std::string Response::generateAutoIndex(const std::string& path,
                                        const std::string& uri)
{
    DIR* dir = opendir(path.c_str());
    if (!dir)
        return buildError(500);

    std::stringstream body;
    body << "<h1>Index of " << uri << "</h1><ul>";

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL)
    {
        std::string name = entry->d_name;

        body << "<li><a href=\""
             << uri;

        if (uri[uri.length() - 1] != '/')
            body << "/";

        body << name << "\">" << name << "</a></li>";
    }

    body << "</ul>";
    closedir(dir);

    Response res;
    res.setBody(body.str());
    res.setContentType("text/html");

    return res.build();
}

std::string Response::buildFileResponse(const std::string& path)
{
    Response res;

    if (!fileExists(path))
        return buildError(404);

    std::string body = readFile(path);

    res.setBody(body);
    res.setContentType(getContentType(path));

    return res.build();
}
