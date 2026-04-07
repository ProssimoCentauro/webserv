#include "Request.hpp"
#include <iostream>


Request::Request(std::string request_buf): request_buf(request_buf)
{
	this->state = REQUEST_LINE;
}

Request::Request(): request_buf("")
{
	this->state = REQUEST_LINE;
}

Request::~Request()
{
	
}

Request::Request(const Request& other): request_buf(other.request_buf)
{
	this->state = other.state;
}

Request& Request::operator=(const Request& other)
{
	if(this != &other)
	{
		this->request_buf = other.request_buf; 
		this->state = other.state;
	}
	return(*this);
}

Request::RequestException::RequestException(int code): code(code)
{

}

int Request::RequestException::getCode() const
{
	return(this->code);
}

const char* Request::RequestException::what() const throw()
{
	return("HTTP Request Error");
}

bool Request::is_space(char c)
{
	return((c >= 7 && c <= 13) || c == 32);
}

bool Request::is_crlf(char c)
{
	return(c == '\r' || c == '\n');
}

bool Request::is_digit(char c)
{
	return(c >= '0' && c <= '9');
}

void Request::setBuffer(std::string buf)
{
	request_buf = buf;
}

bool Request::is_number(std::string str)
{
	for(size_t i = 0; i < str.size(); i++)
	{
		if(str[i] < '0' || str[i] > '9')
			return(false);
	}
	return(true);
}

std::string Request::strTrim(std::string str, size_t pos)
{
	std::string new_str;
	new_str = str.substr(0, pos);
	return(new_str);
}


void Request::cleanTerminator()
{
	size_t i = 0;
	size_t start = 0;
	while(i < request_buf.size() && (is_space(request_buf[i]) || is_crlf(request_buf[i])))
		i++;
	request_buf.erase(start, i - start);
}

std::string Request::extractToken()
{
    size_t i = 0;

    while(i < request_buf.size() && is_space(request_buf[i]))
        i++;

    size_t start = i;

    while(i < request_buf.size() && !is_space(request_buf[i]) && request_buf[i] != '\r')
        i++;

    std::string token = request_buf.substr(start, i - start);

    request_buf.erase(0, i);

    return token;
}

std::string Request::extractHeader(std::string& line)
{
    size_t i = 0;

    while(i < line.size() && (is_space(line[i]) || is_crlf(line[i])))
        i++;

    size_t start = i;

    while(i < line.size() && line[i] != '\r')
    {
        i++;
        if(line[i] == ':' && line[i + 1] && is_space(line[i + 1]))
            break;
    }
    std::string key = line.substr(start, i - start);

    if (i < line.size() && line[i] == ':')
        i++;

    line.erase(0, i);

    return key;
}

void Request::parseRequestLine()
{
    ConfReq.method = extractToken();
    if (ConfReq.method != "GET" && ConfReq.method != "POST" &&
			ConfReq.method != "DELETE")
        throw RequestException(405);

    ConfReq.uri = extractToken();
    if (ConfReq.uri.empty() || ConfReq.uri[0] != '/')
        throw RequestException(400);
    if (ConfReq.uri.size() > 2048)
        throw RequestException(414);

    ConfReq.version = extractToken();
    if (ConfReq.version != "HTTP/1.0" && ConfReq.version != "HTTP/1.1")
        throw RequestException(505);

    cleanTerminator();
}

void Request::parseHeaders(std::string line)
{
        while(!line.empty())
        {
            if(line.find("\r\n") == 0)
            {
                line.erase(0, 2);
                if(line.empty())
                    break;
            }
            size_t line_end = line.find("\n");

            std::string headers = line.substr(0, line_end);
            line.erase(0, headers.size() - 1);


            size_t pos = headers.find(':');
            if (pos == std::string::npos)
                throw RequestException(400);

            std::string key = strToLower(extractHeader(headers));
            std::string value = extractHeader(headers);

            size_t i = 0;
            while (i < value.size() && is_space(value[i]))
                i++;
            value.erase(0, i);

            if (ConfReq.headers.count(key))
                throw RequestException(400);

            if (key == "content-length" && !is_number(value))
                throw RequestException(400);

            ConfReq.headers.insert(std::make_pair(key, value));

            if (ConfReq.headers.count("content-length") &&
                    ConfReq.headers.count("transfer-encoding"))
                throw RequestException(400);
        }
}


void Request::parseBody()
{
	
	if(request_buf.find("\r\n0") == 0 || request_buf.find("0\r\n") == 0)
	{
		state = DONE;
		return;
	}
	size_t pos = request_buf.find("\r\n");
	std::string hex = request_buf.substr(0, pos);
	size_t len = std::strtoul(hex.c_str(), NULL, 16);
	while(request_buf.size() < len + 3)
		return;
	request_buf.erase(0, pos + 2);
	ConfReq.body += request_buf.substr(0, len);
	request_buf.erase(0, len);
}

std::string Request::strToLower(std::string str)
{
	size_t i = 0;
	while(i < str.size())
	{
		if(str[i] >= 'A' && str[i] <= 'Z')
			str[i] = std::tolower(str[i]);
		i++;
	}
	return(str);
}

bool Request::hasContentLength(std::string s)
{
	return(ConfReq.headers.find(s)  != ConfReq.headers.end());
}

std::string Request::GetHeaderLen()
{
	std::map<std::string, std::string>::const_iterator it = ConfReq.headers.find("content-length");
	return(it->second);
}

void Request::parse()
{
    size_t old_size = 0;

    while (!request_buf.empty())
    {
        if (request_buf.size() == old_size)
            break;
        old_size = request_buf.size();

        if (state == REQUEST_LINE)
        {
            size_t pos = request_buf.find("\r\n");
            if (pos == std::string::npos)
                return;

            parseRequestLine();
            state = HEADERS;
        }

        if (state == HEADERS)
        {
            size_t pos = request_buf.find("\r\n\r\n");
            if(pos != std::string::npos)
            {
                std::string line = request_buf.substr(0, pos + 2);
                request_buf.erase(0, pos + 2);
                parseHeaders(line);
            }
            else
                throw RequestException(400);

            if (request_buf.substr(0, 2) == "\r\n")
			{
				request_buf.erase(0, 2);
				state = BODY_CONTENT;
			}
        }

        if (state == BODY_CONTENT && hasContentLength("content-length"))
        {
            size_t n = std::atoi(GetHeaderLen().c_str());

            if (request_buf.size() < n)
                return;

            ConfReq.body = request_buf.substr(0, n);
            request_buf.erase(0, n);
            state = DONE;
        }
        else if (state == BODY_CONTENT)
        {
            state = DONE;
        }

        if (state == DONE)
            return;
    }
}


bool   Request::isDone() const
{
	return(this->state == DONE );
}


const RequestConfig& Request::getReqConf() const
{
	return(ConfReq);
}


//per test

void Request::printHttp()
{
    for(std::map<std::string, std::string>::const_iterator it = ConfReq.headers.begin(); it != ConfReq.headers.end(); ++it)
    {
        std::cout << it->first << " -> " << it->second << std::endl;
    }

    std::cout << "\n" << "*****Print Cookies********" << std::endl;
    for(std::map<std::string, std::string>::const_iterator it = ConfReq.cookie.begin(); it != ConfReq.cookie.end(); ++it)
    {
        std::cout << it->first << " -> " << it->second << std::endl;
    }
}


void Request::parseCookie()
{
    
    for(std::map<std::string, std::string>::const_iterator it = ConfReq.headers.begin(); it != ConfReq.headers.end(); ++it)
    {
        if(it->first == "cookie")
        {
            std::string str = it->second;
            if(!str.empty())
            {
                size_t pos = str.find('=');
                std::string s1 = str.substr(0, pos);
                pos++;
                std::string s2 = str.erase(0, pos);
                ConfReq.cookie.insert(std::make_pair(s1, s2));
            }
        }

    }

}