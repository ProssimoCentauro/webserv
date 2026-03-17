#include "Request.hpp"


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

std::string Request::extractToken(std::string str)
{
	size_t i = 0;
	size_t start = 0;

	while(start < str.size() && is_space(str[start]))
	{
		start++;
		i = start;
	}
	while(i < str.size() && !is_space(str[i]) && str[i] != '\r')
	{
		i++;
	}
	std::string buffer = str.substr(start, i - start);
	request_buf.erase(start, i - start);
	return(buffer);
}

std::string Request::extractHeader(std::string str)
{
	size_t i = 0;
	size_t start = 0;

	while(start < str.size() && (is_space(str[start]) || is_crlf(str[start])))
	{
		start++;
		i = start;
	}
	while(i < str.size() && !is_space(str[i]) && !is_crlf(str[i]))
	{
		i++;
	}
	std::string buffer = str.substr(start, i - start);
	request_buf.erase(start, i - start);
	return(buffer);
}


void Request::parseRequestLine()
{
	ConfReq.method = extractToken(request_buf);
	if(ConfReq.method != "GET" && ConfReq.method != "POST" && ConfReq.method != "DELETE" )
		throw RequestException(405);

	ConfReq.uri = extractToken(request_buf);
	if(ConfReq.uri[0] != '/')
		throw RequestException(400);
	if(ConfReq.uri.size() > 2048)
		throw RequestException(414);  //URI size 

	ConfReq.version = extractToken(request_buf);
	if(ConfReq.version != "HTTP/1.0" && ConfReq.version != "HTTP/1.1")
		throw RequestException(505);

	//puliso terminatori
	cleanTerminator();
	return;
}

void Request::parseHeaders()
{
	std::string key = strToLower(extractHeader(request_buf));

	std::string value = extractHeader(request_buf);

	size_t pos = key.find(':');
	if(pos == std::string::npos)
		throw RequestException(400);

	if(pos != std::string::npos)
	{
		key = strTrim(key, pos);
		request_buf.erase(0, 1); // pulisco lo spazio dopo ':'
	}
	if(ConfReq.headers.count(key)) //se c'è più di un headers uguale
		throw RequestException(400);
	if(key == "content-length")
	{
		if(is_number(value) == false)
			throw RequestException(404);
	}
	ConfReq.headers.insert(std::pair<std::string, std::string>(key, value));

	if(ConfReq.headers.count("content-length") && ConfReq.headers.count("transfer-encoding"))
		throw RequestException(400);
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
	
	std::string buffer;
	size_t pos;
	
	while(!request_buf.empty())
	{
		if(state == REQUEST_LINE )
		{
			pos = request_buf.find("\r\n");
			if(pos != std::string::npos)
			{
				parseRequestLine();
				state = HEADERS;
			}
		}
		if(state == HEADERS )
		{
			pos = request_buf.find("\r\n\r\n");
			if(request_buf.find("\r\n\r\n") == 0)
			{
				cleanTerminator();
				state = BODY_CONTENT;
			}
			if(state == HEADERS && request_buf.find("\r\n") == 0)
				cleanTerminator();
			if (state == HEADERS && request_buf.find("\r\n") != std::string::npos)
				parseHeaders();
		}
		if (state == BODY_CONTENT && hasContentLength("content-length") == true)
			{
				size_t n = std::atoi(GetHeaderLen().c_str());
				ConfReq.body = request_buf.substr(0, n);
				if(ConfReq.body.size() == n || n == 0)
				{
					state = DONE;
					return;
				}
			}
		else if(state == BODY_CONTENT  && hasContentLength("content-length") == false)
		{
			size_t pos = request_buf.find("\r\n");
			if(pos == std::string::npos)
				return;
			state = BODY_CHUNK;
		}
		if(state == BODY_CHUNK)
			parseBody();
		if(state == DONE)
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