#include "WebServer.hpp"


WebServer::WebServer() {}

WebServer::WebServer(const WebServer& other) {*this = other;}

WebServer& WebServer::operator=(const WebServer& other)
{
	if (this != &other)
	{
		this->_config = other._config;
		this->_poller = other._poller;
		this->_listenSockets = other._listenSockets;
		this->_clients = other._clients;
	}
	return *this;
}

WebServer::~WebServer() {}

bool WebServer::checkRequestComplete(const std::string& buffer)
{
	return (buffer.find("\r\n\r\n") != std::string::npos);	
}

void WebServer::acceptClient(int listenFd)
{
	int clientFd = accept(listenFd, NULL, NULL);
	if (clientFd < 0)
		return;
	
	if (fcntl(clientFd, F_SETFL, O_NONBLOCK) < 0)
	{
		close(clientFd);
		return;
	}

	ClientConnection client(clientFd);
	_clients[clientFd] = client;
	_poller.addFd(clientFd, POLLIN);
}
void WebServer::readClient(int clientFd)
{
	char buffer[4096];
	ssize_t readedBytes = recv(clientFd, buffer, sizeof(buffer), 0);
	
	if (readedBytes <= 0)
	{
		closeClient(clientFd);
		return;
	}

	ClientConnection& client = _clients[clientFd];
	client.appendReadBuffer(buffer, readedBytes);
	
	if (checkRequestComplete(client.getReadBuffer()))
	{
		std::string body = "Hello from webserv";

        std::string response;
        response  = "HTTP/1.1 200 OK\r\n";
        response += "Content-Length: ";
        std::stringstream ss;
		ss << body.length();
		response += ss.str();
		response += "\r\n";
        response += "Content-Type: text/plain\r\n";
        response += "Connection: close\r\n";
        response += "\r\n";
        response += body;

		client.getWriteBuffer() = response;

		_poller.setEvents(clientFd, POLLOUT);
		client.setRequestComplete(true);
	}
}

void WebServer::writeClient(int clientFd)
{
	ClientConnection& client = _clients[clientFd];
	std::string& writeBuffer = client.getWriteBuffer();

	if (writeBuffer.empty())
    {
        closeClient(clientFd);
        return;
    }

	ssize_t sentBytes = send(clientFd, writeBuffer.c_str(), writeBuffer.length(), 0);

	if (sentBytes <= 0)
	{
		closeClient(clientFd);
		return;
	}

	client.removeBytes(sentBytes);
	if(writeBuffer.empty())
		closeClient(clientFd);
}

void WebServer::closeClient(int clientFd)
{
	_poller.removeFd(clientFd);
	_clients.erase(clientFd);
	close(clientFd);
}

int WebServer::createListenSocket(int port)
{
	int sockFd = 0;
	int opt = 1;
	sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);


	sockFd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockFd < 0)
		throw std::runtime_error("socket failed");

	setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	
	if (bind(sockFd, (sockaddr *)&addr, sizeof(addr)) < 0)
		throw std::runtime_error("bind failed");

	if (listen(sockFd, 10) < 0)
		throw std::runtime_error("listen failed");
	
	if (fcntl(sockFd, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error("fcntl failed");

	return sockFd;
}


void WebServer::init(const Config& config)
{
	std::vector<ServerConfig> servers = config.getServers();
	
	for (std::vector<ServerConfig>::const_iterator it = servers.begin();
     it != servers.end();
     ++it)
	{
		int listenFd = createListenSocket(it->listen_port);
		_listenSockets.push_back(listenFd);
		_poller.addFd(listenFd, POLLIN);
	}
}

void WebServer::exec()
{
    while (true)
    {
        _poller.wait(-1);

        std::vector<struct pollfd>& fds = _poller.getFds();

        for (size_t i = 0; i < fds.size(); )
        {
            int currentFd = fds[i].fd;
            short revents = fds[i].revents;

            if (std::find(_listenSockets.begin(), _listenSockets.end(), currentFd) != _listenSockets.end())
            {
                if (revents & POLLIN)
                    acceptClient(currentFd);
            }
            else
            {
                if (revents & POLLIN)
                    readClient(currentFd);
                else if (revents & POLLOUT)
                    writeClient(currentFd);
                else if (revents & (POLLERR | POLLHUP | POLLNVAL))
                    closeClient(currentFd);
            }

            if (i < fds.size() && fds[i].fd == currentFd)
                ++i;
        }
    }
}












