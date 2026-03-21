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

void Config::addServer(const ServerConfig& server)
{
    _servers.push_back(server);
}

const std::vector<ServerConfig>& Config::getServers() const
{
	return _servers;
}
