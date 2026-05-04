#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <map>
#include "structures.h"

class Response
{
private:
    int _statusCode;
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

    // ENTRY
    static std::string buildResponse(const RequestConfig& req,
                                     const ServerConfig& server);

    // HANDLERS
    static std::string handleGet(const RequestConfig& req,
                                 const LocationConfig& loc,
                                 const ServerConfig& server);

    static std::string handlePost(const RequestConfig& req,
                                  const LocationConfig& loc);

    static std::string handleDelete(const RequestConfig& req,
                                    const LocationConfig& loc);

    static std::string executeCgi(const RequestConfig& req,
                                  const std::string& interpreter);

    // HELPERS
    static std::string buildError(int code);
    static std::string buildRedirect(int code, const std::string& url);
    static std::string getExtension(const std::string& path);
    static const LocationConfig* matchLocation(const std::string& uri,
                                               const ServerConfig& server);

    // UTILS
    static bool fileExists(const std::string& path);
    static bool deleteFile(const std::string& path);
    static std::string readFile(const std::string& path);
    static bool writeFile(const std::string& path, const std::string& content);
    static std::string getContentType(const std::string& path);
    static std::string getStatusMessage(int code);
	static std::string generateSessionId();
	static std::string generateAutoIndex(const std::string& path, const std::string& uri);
	static std::string buildFileResponse(const std::string& path);

};

#endif
