#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <string>
#include <vector>
#include <map>

typedef struct SLocationConfig
{
    std::string path;                  // "/upload"
    std::vector<std::string> methods;  // GET POST
    std::string root;                  // ./www/upload
    std::string index;                 // index.html
    bool autoindex;                    // on/off
    bool upload_enabled;
    std::string upload_path;
    std::string cgi_extension;         // ".py"
    std::string cgi_path;              // /usr/bin/python3
}LocationConfig;

typedef struct SServerConfig
{
    int listen_port;                           // 8080
    std::string root;                          // ./www
    size_t client_max_body_size;               // limite POST

    std::map<int, std::string> error_pages;    // 404 -> ./404.html

    std::vector<LocationConfig> locations;
}ServerConfig;

#endif
