#include "Config.hpp"

Config::Config() {}

Config::Config(const Config& other) {*this = other;}

Config& Config::operator=(const Config& other) {
    if (this != &other) {
        _servers = other._servers;
    }
    return *this;
}

Config::~Config() {}

const std::vector<ServerConfig>& Config::getServers() const
{
	return _servers;
}