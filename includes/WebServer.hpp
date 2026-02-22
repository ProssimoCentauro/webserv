#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include "WebServ.h"
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
    void exec();

private:
    void acceptClient(int listenFd);
    void readClient(int clientFd);
    void writeClient(int clientFd);
    void closeClient(int clientFd);

	int createListenSocket(int port);

    Config _config;
    Poller _poller;
	std::vector<int> _listenSockets;
    std::map<int, ClientConnection> _clients;
};

#endif

