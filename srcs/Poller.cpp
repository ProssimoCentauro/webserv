#include "Poller.hpp"

Poller::Poller() {}

Poller::Poller(const Poller& other) {*this = other;}

Poller& Poller::operator=(const Poller& other) {
    if (this != &other) {
        _fds = other._fds;
    }
    return *this;
}

Poller::~Poller() {}

void	Poller::addFd(int fd, short events)
{
	struct pollfd s_fd;
	
	s_fd.fd = fd;
	s_fd.events = events;
	s_fd.revents = 0;

	 _fds.push_back(s_fd);
}

void	Poller::removeFd(int fd)
{
	std::vector<struct pollfd>::iterator it;
	
	for (it = _fds.begin(); it != _fds.end();)
	{
		if (it->fd == fd)
		{
			_fds.erase(it);
			return;
		}
		else
			it++;
	}
}

void	Poller::setEvents(int fd, short events)
{
	std::vector<struct pollfd>::iterator it;

	for (it = _fds.begin(); it != _fds.end();)
	{
		if (it->fd == fd)
		{
			it->events = events;
			return;
		}
		else
			it++;
	}
}

int	Poller::wait(int timeout)
{
	if (_fds.empty())
		return 0;
	
	return poll(&_fds[0], _fds.size(), timeout);
}

std::vector<struct pollfd>& Poller::getFds()
{
	return _fds;
}
