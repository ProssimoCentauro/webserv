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

void acceptClient(int listenFd)
{
	ClientConnection conn(listenFd);

	_clients[]
}
void readClient(int clientFd);
void writeClient(int clientFd);
void closeClient(int clientFd);

int createListenSocket(int port)
{
	int sockFd = 0;
	int opt = 1;
	sockaddr_in addr = {0};
	
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);


	sockFd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockFd < 0)
	{
		close(sockFd);
		throw std::runtime_error("socket failed");
	}

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
	int ListenFd;
	
	for (const ServerConfig& s : servers)
	{
		ListenFd = createListenSocket(s.listen_port);
		_listenSockets.push_back(ListenFd);
		_poller.addFd(ListenFd, POLLIN);
	}
}

void WebServer::exec()
{
	std::vector<struct pollfd> fds = _poller.getFds();
	
	while (42)
	{
		_poller.wait(0);

		for (struct pollfd fd : fds)
		{
			if (std::find(_listenSockets.begin(), _listenSockets.end(), fd) != _listenSockets.end())
			{
				if (fd.revents & POLLIN)
					acceptClient(fd.fd);
			}
			else
			{
				if (fd.revents & POLLIN)
					readClient(fd.fd);
				else if (fd.revents & POLLOUT)
					writeClient(fd.fd);
				else if (fd.revents & (POLLERR | POLLHUP | POLLNVAL))
					closeClient(fd.fd);
			}
		}	
	}
}













