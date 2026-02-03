#ifndef POLLER_HPP
#define POLLER_HPP

#include <vector>
#include <poll.h>

class Poller
{
public:
    Poller();
    Poller(const Poller& other);
    Poller& operator=(const Poller& other);
    ~Poller();

    void addFd(int fd, short events);
    void removeFd(int fd);
    void setEvents(int fd, short events);

    int wait(int timeout);
    std::vector<struct pollfd>& getFds();

private:
    std::vector<struct pollfd> _fds;
};

#endif

