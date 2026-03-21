#include "WebServer.hpp"
#include "Config.hpp"
#include <iostream>

int main()
{
    try
    {
        Config config;
        WebServer server;

        server.init(config);

        std::cout << "Webserv running..." << std::endl;

        server.exec();
    }
    catch (std::exception& e)
    {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
