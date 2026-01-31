#include "WebServ.h"
#include <iostream>

int main() {
    WebServer server;

    // ServerConfig
    ServerConfig config;
    config.listen_port = 8080;
    config.root = "./www";
    config.client_max_body_size = 1024 * 1024; // 1 MB

    // LocationConfig
    LocationConfig loc;
    loc.path = "/";
    loc.methods.push_back("GET");
    loc.methods.push_back("POST");
    loc.root = "./www";
    loc.index = "index.html";
    loc.autoindex = false;
    loc.upload_enabled = false;
    loc.cgi_extension = ".py";
    loc.cgi_path = "/usr/bin/python3";

    config.locations.push_back(loc);

    // Adding server to WebServer
    std::vector<ServerConfig> server_list;
    server_list.push_back(config);
    server.setServers(server_list);

    // Debug
    std::vector<ServerConfig> servers = server.getServers();
    std::cout << "Number of servers: " << servers.size() << std::endl;
    if (!servers.empty()) {
        std::cout << "First server port: " << servers[0].listen_port << std::endl;
        std::cout << "Root: " << servers[0].root << std::endl;
        if (!servers[0].locations.empty()) {
            std::cout << "First location path: " << servers[0].locations[0].path << std::endl;
        }
    }

    return 0;
}
