//#include "WebServer.hpp"
//#include "Config.hpp"
#include <iostream>
#include "Request.hpp"
#include "structures.h"

int main()
{

	srand(time(NULL));

	// *****TEST CONTENT_LENGHT BODY**********

	/*std::string str = "POST /upload HTTP/1.1\r\n"
      	"Host: localhost\r\n"
        "ConTenT-Length: 10\r\n"
        "Connection: keep-alive\r\n"
        "\r\n"
       	"abcdefghij";*/
	
	//********TEST BODY ENCODER***********

	std::string str ="POST /upload HTTP/1.1\r\n"
		"Host: localhost\r\n"
		"Transfer-Encoding: chunked\r\n"
		"Connection: keep-alive\r\n"
		"\r\n"
		"5\r\n"
		"Hello\r\n"
		"1\r\n"
		" \r\n"
		"6\r\n"
		"World!\r\n"
		"0\r\n"
		"\r\n";

	//*************TEST CONTENT_LENGTH a 0*************

	/*std::string str = "POST /upload HTTP/1.1\r\n"
		"Host: localhost\r\n"
		"Content-Length: 0\r\n"
		"\r\n";*/
	
	Request r;

	// **************TEST TRANSFER ENCODING CON CHUNK A 0**********
	
	/*std::string str = "POST /upload HTTP/1.1\r\n"
		"Host: localhost\r\n"
		"TRANSFER-Encoding: chunked\r\n"
		"\r\n"
		"0\r\n"
		"\r\n";*/

	
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

		//DECOMMENTARE PER TEST CON HEADER CONTENT_LENGTH 10 E BODY "abcdefghij"
        /*if(c.body != "abcdefghij")
        {
            std::cout << "Errore nel parsing alla run: " << t << std::endl;
            std::cout << "Body ricevuto: " << c.body << std::endl;
            //return 1;
			break;
        }*/


		//DECOMMENTARE PER TEST CON HEADER TRANSEFR ENCODING  E BODY "Hello World!"
		/*if(c.body != "Hello World!")
        {
            std::cout << "Errore nel parsing alla run: " << t << std::endl;
            std::cout << "Body ricevuto: " << c.body << std::endl;
            //return 1;
			break;
        }*/
    }
    return 0;
}

