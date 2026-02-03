#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <map>

class Request
{
public:
    Request();
    Request(const Request& other);
    Request& operator=(const Request& other);
    ~Request();

    void append(const std::string& data);
    bool isComplete() const;

    const std::string& getMethod() const;
    const std::string& getPath() const;
    const std::string& getVersion() const;
    const std::map<std::string, std::string>& getHeaders() const;
    const std::string& getBody() const;

private:
    void parse();

    std::string _raw;
    std::string _method;
    std::string _path;
    std::string _version;
    std::map<std::string, std::string> _headers;
    std::string _body;
    bool _parsed;
};

#endif

