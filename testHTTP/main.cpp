//#include "WebServer.hpp"
//#include "Config.hpp"
#include <iostream>
#include "Request.hpp"
#include "structures.h"

int main()
{
    /*try
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
    }*/

	srand(time(NULL));

	std::string str = "POST /upload HTTP/1.1\r\n"
      	"Host: localhost\r\n"
        "Content-Length: 10\r\n"
        "Connection: keep-alive\r\n"
        "\r\n"
       	"abcdefghij";
	
	
	Request r;
	/*size_t chunk = 6;

	for(size_t i = 0; i < str.size(); i+= chunk)
	{
		size_t len = chunk;
		if(i + len > str.size())
			len = str.size() - i;
		std::string chunk = str.substr(i, len);
		r.setBuffer(chunk);
		r.parse();
	}*/

	
    for(int t = 0; t < 1000; t++)
    {
       // Request r;

        for(size_t i = 0; i < str.size(); )
        {
            size_t chunk = rand() % 5 + 1;

            size_t len = chunk;
            if(i + len > str.size())
                len = str.size() - i;

            std::string piece = str.substr(i, len);

            r.setBuffer(piece);
            r.parse();

            i += len;
        }
		const RequestConfig& c = r.getReqConf();


		std::cout << "method: " << c.method << std::endl;
		std::cout << "uri: " << c.uri << std::endl;
		std::cout << "version: " << c.version << std::endl;

		for(std::map<std::string, std::string>::const_iterator it = c.headers.begin(); it != c.headers.end(); ++it)
		{
			std::cout << it->first << " -> " << it->second << std::endl;
		}

		std::cout << "body:" << c.body << std::endl;
        if(c.body != "abcdefghij")
        {
            std::cout << "Errore nel parsing alla run: " << t << std::endl;
            std::cout << "Body ricevuto: " << c.body << std::endl;
            //return 1;
			break;
        }
    }


	/*const RequestConfig& c = r.getReqConf();

	std::cout << "method: " << c.method << std::endl;
	std::cout << "uri: " << c.uri << std::endl;
	std::cout << "version: " << c.version << std::endl;

	for(std::map<std::string, std::string>::const_iterator it = c.headers.begin(); it != c.headers.end(); ++it)
	{
		std::cout << it->first << " -> " << it->second << std::endl;
	}

	std::cout << "body: " << c.body << std::endl;*/

    return 0;
}

