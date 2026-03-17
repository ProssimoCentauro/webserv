#ifndef CLIENTCONNECTION_HPP
#define CLIENTCONNECTION_HPP

#include <string>
#include "WebServ.h"
#include "Request.hpp"

class ClientConnection
{
public:
    ClientConnection();
    ClientConnection(int fd);
    ClientConnection(const ClientConnection& other);
    ClientConnection& operator=(const ClientConnection& other);
    ~ClientConnection();

    int getFd() const;

    std::string& getReadBuffer();
    std::string& getWriteBuffer();

	void appendReadBuffer(char *buffer, ssize_t bytes);
	void appendWriteBuffer(char *buffer, ssize_t bytes);
	
	void removeBytes(ssize_t	bytes);

    bool requestComplete() const;
    void setRequestComplete(bool value);
	Request& getRequest();

private:
    int _fd;
    std::string _readBuffer;
    std::string _writeBuffer;

	Request request;
    bool _requestComplete;
};

#endif

