#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>
#include "structures.h"

class Config
{
public:
    Config();
    Config(const Config& other);
    Config& operator=(const Config& other);
    ~Config();

    const std::vector<ServerConfig>& getServers() const;
    void addServer(const ServerConfig& server);

private:
    std::vector<ServerConfig> _servers;
};

#endif