#include "Config.hpp"

Config::Config()
{
    ServerConfig server;

    server.listen_port = 8080;
    server.root = "./www";
    server.client_max_body_size = 1000000;

    server.error_pages[404] = "./www/404.html";

    LocationConfig location;
    location.path = "/";
    location.root = "./www";
    location.index = "index.html";
    location.autoindex = false;
    location.upload_enabled = false;
    location.upload_path = "";
    location.cgi_extension = "";
    location.cgi_path = "";

    location.methods.push_back("GET");
    location.methods.push_back("POST");

    server.locations.push_back(location);

    _servers.push_back(server);
}

Config::Config(const Config& other) {*this = other;}

Config& Config::operator=(const Config& other) {
    if (this != &other) {
        _servers = other._servers;
    }
    return *this;
}

Config::~Config() {}

const std::vector<ServerConfig>& Config::getServers() const
{
	return _servers;
}
