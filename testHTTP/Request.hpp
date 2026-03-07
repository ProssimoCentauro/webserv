#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <map>
#include "structures.h"

class Request 
{
	private:
			std::string request_buf;
			std::string append_buf;
			RequestConfig ConfReq;
		    //std::vector<std::string> token;
	
	public:
			Request(std::string request_buf, std::string append_buf);
			Request();
			Request(const Request& other);
			Request& operator=(const Request& other);
			~Request();
			void setBuffer(std::string buf);
			void parseRequestLine();
			void parseHeaders();
			void parseBody();
			void parse();
			std::string extractToken(std::string str);
			bool is_space(char c);
			bool is_crlf(char c);
			bool is_digit(char c);
			void cleanTerminator();
			std::string strToLower(std::string str);
			const RequestConfig& getReqConf() const;

			bool hasContentLength(std::string s);
			std::string GetHeaderLen();
			std::string extractHeader(std::string str);
		
};


#endif

