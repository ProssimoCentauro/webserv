#include "Lexer.hpp"
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "Config.hpp" // per test
#include "Parser.hpp" // per test
#include "WebServer.hpp"






int main(int ac, char **av)
{

	if(ac != 2)
	{
		//print error;
		return(1);
	}
	int fd = open(av[1], O_RDONLY);
	if (fd < 0)
	{
		//print error
		close(fd);
		return(1);
	}
	struct stat st;
	if(stat(av[1], &st) < 0)
	{
		//print error
		close(fd);
		return(1);
	}
	size_t size = st.st_size;
	char *buffer = new char[size];
	ssize_t b_read = read(fd, buffer, size);
	if(b_read < 0)
	{
		//print error;
		delete[] buffer;
		close(fd);
		return(1);
	}
	close(fd);


	Config config;

	try
	{
		Lexer lexer(buffer, size);
		lexer.tokenizer();
		lexer.printTok();
		lexer.assignType();
		std::cout << "==========================================="<< std::endl;
		std::cout << "==========================================="<< std::endl;
		std::cout << "==========================================="<< std::endl << std::endl;
		lexer.printTok();
		std::cout << std::endl << "*******************************************"<< std::endl;
		std::cout << "==========================================="<< std::endl;
		std::cout << "*******************************************"<< std::endl << std::endl;
		std::vector<Token> t = lexer.getToken();
		Parser paramParse(t);
		paramParse.parser();
		config = paramParse.getConfig();
		std::cout << "\n" << "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*" << std::endl;
		std::cout << "PRIMA DELLA STAMPA" << std::endl;
		paramParse.printConfig();
		std::cout << "\n" << "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*" << std::endl;
		std::cout << "FINISH CONFIG FILE" << std::endl;

		WebServer server;
		server.init(config);
		std::cout << "webserv running..." << std::endl;
		server.exec();
	}
	catch(const std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}




	
	delete[] buffer;	

}



