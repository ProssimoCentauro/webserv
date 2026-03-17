//#include "WebServer.hpp"
//#include "Config.hpp"
#include <iostream>
#include "Request.hpp"
#include "structures.h"

int main()
{


	// *****TEST CONTENT_LENGHT BODY**********

	/*std::string str = "POST /upload HTTP/1.1\r\n"
      	"Host: localhost\r\n"
        "ConTenT-Length: 10\r\n"
		//"ConTenT-Length: 30\r\n" // Test più content-lenght
        "Connection: keep-alive\r\n"
        "\r\n"
       	"abcdefghij";*/
	
	//********TEST BODY ENCODER***********

	/*std::string str ="GET /upload HTTP/1.1\r\n"
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
		"\r\n";*/


		//*************TEST CONTENT_LENGTH a 0*************

		/*std::string str = "POST /upload HTTP/1.1\r\n"
		"Host: localhost\r\n"
		"Content-Length: 0\r\n"
		"\r\n";*/

		// **************TEST TRANSFER ENCODING CON CHUNK A 0**********
	
	std::string str = "POST /upload HTTP/1.1\r\n"
		"Host: localhost\r\n"
		"TRANSFER-Encoding: chunked\r\n"
		"\r\n"
		"0\r\n"
		"\r\n";

		Request r;

		try
		{
			r.setBuffer(str);
			r.parse();

			const RequestConfig& c = r.getReqConf();


			std::cout << "method: " << c.method << std::endl;
			std::cout << "uri: " << c.uri << std::endl;
			std::cout << "version: " << c.version << std::endl;

			for(std::map<std::string, std::string>::const_iterator it = c.headers.begin(); it != c.headers.end(); ++it)
			{
				std::cout << it->first << " -> " << it->second << std::endl;
			}

			std::cout << "body:" << c.body << std::endl;
		}
		catch(std::exception &e)
		{
			std::cout << e.what() << std::endl;
		}




}
	