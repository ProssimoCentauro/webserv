#include "WebServer.hpp"

// Constructors / Destructor
WebServer::WebServer() {
    std::cout << "WebServer constructed" << std::endl;
}

WebServer::WebServer(const WebServer& other) {
    *this = other;
    std::cout << "WebServer copied" << std::endl;
}

WebServer& WebServer::operator=(const WebServer& other) {
    if (this != &other) {
        _servers = other._servers;
    }
    std::cout << "WebServer assigned" << std::endl;
    return *this;
}

WebServer::~WebServer() {
    std::cout << "WebServer destructed" << std::endl;
}

// Get / Set
std::vector<ServerConfig> WebServer::getServers() const {
    return _servers;
}

void WebServer::setServers(const std::vector<ServerConfig>& servers) {
    _servers = servers;
}

// Server management
void WebServer::addServer(const ServerConfig& server) {
    _servers.push_back(server);
}

bool WebServer::removeServerByPort(int port) {
    for (size_t i = 0; i < _servers.size(); ++i) {
        if (_servers[i].listen_port == port) {
            _servers.erase(_servers.begin() + i);
            return true;
        }
    }
    return false;
}

ServerConfig* WebServer::getServerByPort(int port) {
    for (size_t i = 0; i < _servers.size(); ++i) {
        if (_servers[i].listen_port == port)
            return &_servers[i];
    }
    return NULL;
}

// Location management
void WebServer::addLocation(int serverIndex, const LocationConfig& location) {
    if (serverIndex < 0 || serverIndex >= (int)_servers.size()) return;
    _servers[serverIndex].locations.push_back(location);
}

bool WebServer::removeLocation(int serverIndex, const std::string& path) {
    if (serverIndex < 0 || serverIndex >= (int)_servers.size()) return false;
    std::vector<LocationConfig>& locations = _servers[serverIndex].locations;
    for (size_t i = 0; i < locations.size(); ++i) {
        if (locations[i].path == path) {
            locations.erase(locations.begin() + i);
            return true;
        }
    }
    return false;
}

LocationConfig* WebServer::getLocation(int serverIndex, const std::string& path) {
    if (serverIndex < 0 || serverIndex >= (int)_servers.size()) return NULL;
    std::vector<LocationConfig>& locations = _servers[serverIndex].locations;
    for (size_t i = 0; i < locations.size(); ++i) {
        if (locations[i].path == path)
            return &locations[i];
    }
    return NULL;
}

// Debug / Utility
void WebServer::printConfig() const {
    std::cout << "WebServer has " << _servers.size() << " server(s)" << std::endl;
    for (size_t i = 0; i < _servers.size(); ++i) {
        const ServerConfig& server = _servers[i];
        std::cout << "Server " << i 
                  << " port: " << server.listen_port
                  << " root: " << server.root
                  << " max_body_size: " << server.client_max_body_size
                  << std::endl;
        for (size_t j = 0; j < server.locations.size(); ++j) {
            const LocationConfig& loc = server.locations[j];
            std::cout << "  Location " << loc.path
                      << " root: " << loc.root
                      << " index: " << loc.index
                      << " autoindex: " << loc.autoindex
                      << " upload_enabled: " << loc.upload_enabled
                      << " cgi_path: " << loc.cgi_path
                      << std::endl;
        }
    }
}
