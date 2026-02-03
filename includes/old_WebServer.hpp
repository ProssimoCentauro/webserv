#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include "structures.h"

// WebServer class: manages multiple servers and their locations
class WebServer {
public:
    // Constructors and destructor
    WebServer();
    WebServer(const WebServer& other);
    WebServer& operator=(const WebServer& other);
    ~WebServer();

    // Get/Set server list
    std::vector<ServerConfig> getServers() const;
    void setServers(const std::vector<ServerConfig>& servers);

    // Server management
    void addServer(const ServerConfig& server);
    bool removeServerByPort(int port);
    ServerConfig* getServerByPort(int port);

    // Location management
    void addLocation(int serverIndex, const LocationConfig& location);
    bool removeLocation(int serverIndex, const std::string& path);
    LocationConfig* getLocation(int serverIndex, const std::string& path);

    // Utility / debug
    void printConfig() const;

private:
    std::vector<ServerConfig> _servers;
};

#endif
