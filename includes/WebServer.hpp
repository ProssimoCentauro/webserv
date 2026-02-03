#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include <map>
#include "Poller.hpp"
#include "Config.hpp"
#include "ClientConnection.hpp"

class WebServer
{
public:
    WebServer();
    WebServer(const WebServer& other);
    WebServer& operator=(const WebServer& other);
    ~WebServer();

    void init(const Config& config);
    void run();

private:
    void acceptClient(int listenFd);
    void readClient(int clientFd);
    void writeClient(int clientFd);
    void closeClient(int clientFd);

    Config _config;
    Poller _poller;
    std::map<int, ClientConnection> _clients;
};

#endif

