#include "lexer.hpp"
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "Config.hpp" // per test
#include "parser.hpp" // per test




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
	paramParse.printConfig();

	
	delete[] buffer;	

}
