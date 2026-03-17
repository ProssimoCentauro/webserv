#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <map>

class Response
{
public:
    Response();
    Response(const Response& other);
    Response& operator=(const Response& other);
    ~Response();

    void setStatus(int code);
    void setHeader(const std::string& key, const std::string& value);
    void setBody(const std::string& body);

    std::string serialize() const;

private:
    int _status;
    std::map<std::string, std::string> _headers;
    std::string _body;
};

#endif

