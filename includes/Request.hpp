#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <map>
#include "structures.h"
#include <cstdlib>

class Request 
{
	private:
			std::string request_buf;
			RequestConfig ConfReq;
			typedef enum e_state
			{
				REQUEST_LINE,
				HEADERS,
				BODY_CONTENT,
				BODY_CHUNK,
				DONE
			} Requeststate;
			Requeststate state;
	
	public:
			Request(std::string request_buf);
			Request();
			Request(const Request& other);
			Request& operator=(const Request& other);
			~Request();
			void setBuffer(std::string buf);
			void parseRequestLine();
			void parseHeaders();
			void parseBody();
			void parse();
			std::string extractToken();
			bool is_space(char c);
			bool is_crlf(char c);
			bool is_digit(char c);
			void cleanTerminator();
			std::string strToLower(std::string str);
			const RequestConfig& getReqConf() const;

			bool hasContentLength(std::string s);
			std::string GetHeaderLen();
			std::string extractHeader();
			bool is_number(std::string str);
			std::string strTrim(std::string str, size_t pos);
			bool isDone() const;

			class RequestException : public std::exception
			{
				private:
						int code;
				public:
						RequestException(int code);
						int getCode() const; 
						virtual const char* what() const throw();
			};
		
};


#endif

