#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include "WebServ.h"
#include "Poller.hpp"
#include "Config.hpp"
#include "ClientConnection.hpp"
#include "Response.hpp"
#include "token.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"

class WebServer
{
public:
    WebServer();
    WebServer(char *input, size_t size);
    WebServer(const WebServer& other);
    WebServer& operator=(const WebServer& other);
    ~WebServer();

    void init(const Config& config);
    void run();
    std::vector<Token> getToken() const;
    void exec();
    void closeAllSockets() const;


private:
    void acceptClient(int listenFd);
    void readClient(int clientFd);
    void writeClient(int clientFd);
    void closeClient(int clientFd);


	int createListenSocket(int port);
	bool checkRequestComplete(const std::string& buffer);

	void	signal_handler(void);

    Config _config;
    Lexer _lex;
    Parser _parse;       
    Poller _poller;
	std::vector<int> _listenSockets;
    std::map<int, ClientConnection> _clients;
	std::map<int, const ServerConfig*> _socketToServer;
};

#endif

