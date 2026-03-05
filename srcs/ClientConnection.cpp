#include "ClientConnection.hpp"


ClientConnection::ClientConnection() {}

ClientConnection::ClientConnection(int fd) : _fd(fd), _requestComplete(false) {}

ClientConnection::ClientConnection(const ClientConnection& other)
{
	*this = other;
}

ClientConnection& ClientConnection::operator=(const ClientConnection& other)
{
	if (this != &other)
	{
		_fd = other._fd;
		_readBuffer = other._readBuffer;
		_writeBuffer = other._writeBuffer;
		_requestComplete = other._requestComplete;
	}
	return *this;
}

ClientConnection::~ClientConnection() {}

int ClientConnection::getFd() const
{
	return _fd;
}

std::string& ClientConnection::getReadBuffer()
{
	return _readBuffer;
}

std::string& ClientConnection::getWriteBuffer()
{
	return _writeBuffer;
}

void ClientConnection::appendReadBuffer(char *buffer, ssize_t bytes)
{
	_readBuffer.append(buffer, bytes);
}
void ClientConnection::appendWriteBuffer(char *buffer, ssize_t bytes)
{
	_writeBuffer.append(buffer, bytes);
}
	
void ClientConnection::removeBytes(ssize_t bytes)
{
	_writeBuffer.erase(0, bytes);
}

bool ClientConnection::requestComplete() const
{
	return _requestComplete;
}

void ClientConnection::setRequestComplete(bool value)
{
	_requestComplete = value;
}
