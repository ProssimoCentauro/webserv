#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <map>
#include "Request.hpp"

class Response
{
private:
    int         _statusCode;
    std::string _statusMessage;
    std::string _body;
    std::string _contentType;
    std::map<std::string, std::string> _headers;

public:
    Response();

    void setStatus(int code, const std::string& message);
    void setBody(const std::string& body);
    void setContentType(const std::string& type);
    void addHeader(const std::string& key, const std::string& value);

    std::string build() const;

    static std::string buildResponse(const RequestConfig& req);

private:
    static std::string handleGet(const RequestConfig& req);
    static std::string handlePost(const RequestConfig& req);
    static std::string handleDelete(const RequestConfig& req);
	
	static std::string executeCgi(const RequestConfig& req);

    static std::string buildError(int code);

    static bool fileExists(const std::string& path);
    static bool deleteFile(const std::string& path);
    static std::string readFile(const std::string& path);
    static bool writeFile(const std::string& path, const std::string& content);

    static std::string getContentType(const std::string& path);
    static std::string getStatusMessage(int code);

    static std::string generateSessionId();
};

#endif
