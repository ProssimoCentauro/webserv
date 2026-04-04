#include "Response.hpp"
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdlib>
#include <fcntl.h>
#include <sys/wait.h>

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

std::string Response::buildResponse(const RequestConfig& req)
{
    if (req.method == "GET")
        return handleGet(req);
    if (req.method == "POST")
        return handlePost(req);
    if (req.method == "DELETE")
        return handleDelete(req);

    return buildError(405);
}

/* ================= GET ================= */

std::string Response::handleGet(const RequestConfig& req)
{
    // CGI detection
    if (req.uri.find("/cgi-bin/") == 0)
        return executeCgi(req);

    Response res;
    std::string path = "./www";

    if (req.uri == "/")
        path += "/index.html";
    else
        path += req.uri;

    if (!fileExists(path))
        return buildError(404);

    std::string body = readFile(path);
    if (body.empty())
        return buildError(500);

    res.setBody(body);
    res.setContentType(getContentType(path));

    res.addHeader("Set-Cookie", "session=" + generateSessionId());

    return res.build();
}

/* ================= POST ================= */

std::string Response::handlePost(const RequestConfig& req)
{
    // CGI
    if (req.uri.find("/cgi-bin/") == 0)
        return executeCgi(req);

    Response res;

    std::string path = "./www/uploads/upload.txt";

    if (!writeFile(path, req.body))
        return buildError(500);

    res.setStatus(201, "Created");
    res.setBody("<h1>Upload OK</h1>");

    return res.build();
}

/* ================= DELETE ================= */

std::string Response::handleDelete(const RequestConfig& req)
{
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

std::string Response::executeCgi(const RequestConfig& req)
{
    int pipefd[2];
    if (pipe(pipefd) == -1)
        return buildError(500);

    pid_t pid = fork();
    if (pid < 0)
        return buildError(500);

    if (pid == 0)
    {
        // CHILD

        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);

        std::string scriptPath = "./www" + req.uri;

        char *argv[2];
        argv[0] = (char*)scriptPath.c_str();
        argv[1] = NULL;

        // ENV
        std::string method = "REQUEST_METHOD=" + req.method;

		std::string contentLength = "CONTENT_LENGTH=0";

		std::map<std::string,std::string>::const_iterator it =
		req.headers.find("content-length");

		if (it != req.headers.end())
			contentLength = "CONTENT_LENGTH=" + it->second;

        char *envp[3];
        envp[0] = (char*)method.c_str();
        envp[1] = (char*)contentLength.c_str();
        envp[2] = NULL;

        execve(scriptPath.c_str(), argv, envp);
        exit(1);
    }

    // PARENT
    close(pipefd[1]);

    char buffer[4096];
    std::stringstream output;
    ssize_t bytes;

    while ((bytes = read(pipefd[0], buffer, sizeof(buffer))) > 0)
        output.write(buffer, bytes);

    close(pipefd[0]);
    waitpid(pid, NULL, 0);

    Response res;
    res.setBody(output.str());
    res.setContentType("text/html");

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
