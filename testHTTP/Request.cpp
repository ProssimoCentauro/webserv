#include "Request.hpp"

int flag = 1;

Request::Request(std::string request_buf, std::string append_buf): request_buf(request_buf), append_buf(append_buf)
{
	
}

Request::Request(): request_buf(""), append_buf("")
{
	
}

Request::~Request()
{
	
}

Request::Request(const Request& other): request_buf(other.request_buf), append_buf(other.append_buf)
{

}

Request& Request::operator=(const Request& other)
{
	if(this != &other)
	{
		this->request_buf = other.request_buf; 
		this->append_buf = other.append_buf;
	}
	return(*this);
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
	return(c >= 0 && c <= 9);
}

void Request::setBuffer(std::string buf)
{
	request_buf += buf;
}


void Request::cleanTerminator()
{
	int i = 0;
	int start = 0;
	while(is_space(request_buf[i]) || is_crlf(request_buf[i]))
		i++;
	request_buf.erase(start, i - start);
}

std::string Request::extractToken(std::string str)
{
	int i = 0;
	int start = 0;

	while(is_space(str[start]))
	{
		start++;
		i = start;
	}
	while(!is_space(str[i]) && str[i] != '\r')
	{
		i++;
	}
	std::string buffer = str.substr(start, i - start);
	request_buf.erase(start, i - start);
	return(buffer);
}

std::string Request::extractHeader(std::string str)
{
	int i = 0;
	int start = 0;

	while(is_space(str[start]) || is_crlf(str[start]))
	{
		start++;
		i = start;
	}
	while(!is_space(str[i]) && !is_crlf(str[i]) && str[i] != ':')
	{
		i++;
	}
	std::string buffer = str.substr(start, i - start);
	if(str[i] == ':')
	{
		request_buf.erase(start, (i + 1) - start);  //per togliere i ":"
		while(!request_buf.empty() && request_buf[0] == ' ')
        	request_buf.erase(0,1);
	}
	else
		request_buf.erase(start, i - start);
	return(buffer);
}


void Request::parseRequestLine()
{
	ConfReq.method = extractToken(request_buf);
	ConfReq.uri = extractToken(request_buf);
	ConfReq.version = extractToken(request_buf);
	//puliso terminatori
	cleanTerminator();
	return;
}

void Request::parseHeaders()
{
	std::string key = extractHeader(request_buf);
	std::string value = extractHeader(request_buf);
	ConfReq.headers.insert(std::pair<std::string, std::string>((strToLower(key)), value));
}

void Request::parseBody()
{

}

std::string Request::strToLower(std::string str)
{
	size_t i = 0;
	while(str[i])
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
	int j = 0;
	
	while(request_buf[j])
	{
		if(flag == 1 && request_buf.find("\r\n") == std::string::npos)
		{
				return;
		}
		else if(flag == 1 && request_buf.find("\r\n") != std::string::npos)
		{
			parseRequestLine();
			flag = 2;
		}
		if(flag == 2)
		{
			while(request_buf.find("\r\n\r\n") == std::string::npos)
				return;
			if(request_buf.find("\r\n\r\n") == 0)
			{
				cleanTerminator();
				flag = 3;
				//return;
			}
			if (flag == 2 && request_buf.find("\r\n") == std::string::npos)
		   		return;
			else if(flag == 2 && request_buf.find("\r\n") == 0)
				cleanTerminator();
			else if (flag == 2 && request_buf.find("\r\n") != std::string::npos)
				parseHeaders();
		
		}
		if (flag == 3 && hasContentLength("content-length") == true)
		{
			int n = std::atoi(GetHeaderLen().c_str());
			ConfReq.body = request_buf.substr(0, n);
		}
		j++;
	}
}

/*void Request::parse()      // non funziona
{
	
	std::string buffer;
	//int j = 0;
	
	size_t pos = request_buf.find("\r\n");
	if(flag == 1)
	{
		if(pos  == std::string::npos)
			return;
		else if(pos != std::string::npos)
		{
			parseRequestLine();
			flag = 2;
		}
	}
	if(flag == 2)
	{
		size_t pos = request_buf.find("\r\n\r\n");
		if(pos == std::string::npos)
			return;
		parseHeaders();
		request_buf.erase(0, pos + 4);
		flag = 3;
		
	}
	if (flag == 3 && hasContentLength("content-length") == true)
	{
		size_t n = std::atoi(GetHeaderLen().c_str());
		if(request_buf.size() < n)
			return;
		ConfReq.body = request_buf.substr(0, n);
		request_buf.erase(0, n);
		flag = 4;
	}
}*/


const RequestConfig& Request::getReqConf() const
{
	return(ConfReq);
}