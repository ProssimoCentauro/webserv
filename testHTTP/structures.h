#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <string>
#include <vector>
#include <map>

typedef struct s_location_config
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

    std::map<std::string, std::string> cgi;
    size_t client_max_body_size;
    std::map<int, std::string> redirect;

} LocationConfig;

typedef struct s_server_config
{
    int listen_port;                           // 8080
    std::string server_name;                      // piu server ?
    //std::vector<int>listen_port;          //prova per piu listen come string da convertire successivamente
    std::string root;                          // ./www
    size_t client_max_body_size;               // limite POST

    std::map<int, std::string> error_pages;    // 404 -> ./404.html

    std::vector<LocationConfig> locations;
} ServerConfig;


typedef struct s_request
{
	std::string method;
	std::string uri;
	std::string version;
	std::map<std::string, std::string> headers;
	std::string body;
} RequestConfig;

/*typedef struct e_state
{
	REQUEST_LINE,
	HEADERS,
	BODY,
	END

} RequestState;
*/
#endif