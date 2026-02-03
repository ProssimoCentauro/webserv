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
