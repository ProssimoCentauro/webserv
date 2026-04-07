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
    ssize_t bytes = recv(clientFd, buffer, sizeof(buffer), 0);

    if (bytes <= 0)
    {
        closeClient(clientFd);
        return;
    }

    ClientConnection& client = _clients[clientFd];
    client.appendReadBuffer(buffer, bytes);

    try
    {
        Request req;
        req.setBuffer(client.getReadBuffer());

        try
        {
            req.parse();
            req.parseCookie();
        }
        catch (const Request::RequestException &e)
        {
            std::cout << "Request parsing error\n";

            std::string response;
            response  = "HTTP/1.1 400 Bad Request\r\n";
            response += "Content-Length: 0\r\n";
            response += "Connection: close\r\n\r\n";

            client.getWriteBuffer() = response;
            _poller.setEvents(clientFd, POLLOUT);
            return;
        }

        // non completeed request -> wait recv
        if (!req.isDone())
            return;

        // DEBUG
        std::cout << "REQUEST COMPLETE\n";
        std::cout << client.getReadBuffer() << std::endl;

        const RequestConfig& conf = req.getReqConf();

        //DEBUG
        std::cout << "******************* TEST HEADERS **********" << std::endl;
        req.printHttp();

        std::string response = Response::buildResponse(conf);

        client.getWriteBuffer() = response;
        _poller.setEvents(clientFd, POLLOUT);

        // ⚠️ buffer reset
        client.getReadBuffer().clear();
    }
    catch (...)
    {
        closeClient(clientFd);
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












