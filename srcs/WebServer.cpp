#include "WebServer.hpp"

volatile sig_atomic_t WebServer::_serverState = 1;

WebServer::WebServer(){}

WebServer::WebServer(char *input, size_t size): _lex(input, size)
{
   
}

WebServer::WebServer(const WebServer& other) {*this = other;}

WebServer& WebServer::operator=(const WebServer& other)
{
	if (this != &other)
	{
		this->_config = other._config;
		this->_poller = other._poller;
		this->_listenSockets = other._listenSockets;
		this->_clients = other._clients;
        this->_parse = other._parse;
        this->_lex = other._lex;
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
	std::cout << "ACCEPT ON FD=" << listenFd << std::endl;
	int clientFd = accept(listenFd, NULL, NULL);
	if (clientFd < 0)
		return;
	
	if (fcntl(clientFd, F_SETFL, O_NONBLOCK) < 0)
	{
		close(clientFd);
		return;
	}

	ClientConnection client(clientFd);

    std::map<int, const ServerConfig*>::iterator it = _socketToServer.find(listenFd);
    if (it == _socketToServer.end())
    {
        std::cerr << "ERROR: no server for socket" << std::endl;
        close(clientFd);
        return;
    }

    client.setServer(it->second);

	_clients[clientFd] = client;
	_poller.addFd(clientFd, POLLIN);
}

void WebServer::readClient(int clientFd)
{
    char buffer[4096];
    ssize_t bytes = recv(clientFd, buffer, sizeof(buffer), 0);
    const std::vector<ServerConfig>& servers = _config.getServers();


    if (bytes <= 0)
    {
        closeClient(clientFd);
        return;
    }

    ClientConnection& client = _clients[clientFd];
    client.appendReadBuffer(buffer, bytes);

    try
    {
        if (!checkRequestComplete(client.getReadBuffer()))
            return;

        Request req;
        req.setBuffer(client.getReadBuffer());
        req.getMaxBodySize(client.getServer()->client_max_body_size);

        try
        {
            req.parse();
            req.parseCookie();
        }
        catch (const Request::RequestException &e)
        {
            std::cout << "Request parsing error: " << e.getCode() << std::endl;

            std::stringstream response;
            //response += "HTTP/1.1 400 Bad Request\r\n";
            response  << "HTTP/1.1 " << e.getCode() << " "
                      << Response::getStatusMessage(e.getCode()) << "\r\n"
                      << "Content-Length: 0\r\n"
                      << "Connection: close\r\n\r\n";

            client.getWriteBuffer() = response.str();
            _poller.setEvents(clientFd, POLLOUT);

            return;
        }


        // DEBUG
        std::cout << "BUFFER:\n" << client.getReadBuffer() << std::endl;
        std::cout << "REQUEST COMPLETE ✔" << std::endl;

        const RequestConfig& conf = req.getReqConf();

        //DEBUG
        std::cout << "******************* TEST HEADERS **********" << std::endl;
        req.printHttp();

		if (servers.empty())
		{
            std::cout << "URI: [" << conf.uri << "]" << std::endl;
			closeClient(clientFd);
			return;
		}

		const ServerConfig* srv = _clients[clientFd].getServer();
		if (!srv)
		{
			std::cout << "SERVER NULL\n";
			closeClient(clientFd);
			return;
		}

		const ServerConfig& server = *srv;
                
        
		std::string response = Response::buildResponse(conf, server);

        client.getWriteBuffer() = response;
        _poller.setEvents(clientFd, POLLOUT);

        client.getReadBuffer().clear();
    }
    catch (std::exception &e)
    {
        std::cout << "FATAL ERROR: " << e.what() << std::endl;

        std::string response;
        response  = "HTTP/1.1 500 Internal Server Error\r\n";
        response += "Content-Length: 0\r\n";
        response += "Connection: close\r\n\r\n";

        client.getWriteBuffer() = response;
        _poller.setEvents(clientFd, POLLOUT);
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
    _config = config;
    
	const std::vector<ServerConfig>& servers = config.getServers();
	
	for (std::vector<ServerConfig>::const_iterator it = servers.begin();
     it != servers.end();
     ++it)
	{
		int listenFd = createListenSocket(it->listen_port);
		_listenSockets.push_back(listenFd);
		_poller.addFd(listenFd, POLLIN);

		_socketToServer[listenFd] = &(*it);
		std::cout << "MAP INSERT: fd=" << listenFd << std::endl;
	}
}

void WebServer::signal_handler(int signal)
{
    (void)signal;
    write(1, "\n", 1);
    _serverState = 0;
}

void WebServer::run()
{
    signal(SIGINT,signal_handler);
    while (_serverState)
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
    if (!_serverState)
        closeAllSockets();
}

std::vector<Token> WebServer::getToken() const
{
    return _lex.getToken();
}

void WebServer::exec()
{
    _lex.tokenizer();
    _lex.printTok();  // printf for debug
    _lex.assignType();
    std::vector<Token> t = getToken();
    _parse.setToken(t);
    _parse.parser();
    _config = _parse.getConfig();
    _parse.printConfig(); //printf for debug
    init(_config);
    run();
}

void WebServer::closeAllSockets() const
{
    int len = _listenSockets.size();
    for(int i = 0; i < len; i++)
        close(_listenSockets[i]);
};
