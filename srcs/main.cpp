#include "Lexer.hpp"
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "WebServer.hpp"


int main(int ac, char **av)
{

	if(ac != 2)
	{
		std::cerr << "Usage: " << av[0] << " <filename>" << std::endl;
		return(1);
	}
	int fd = open(av[1], O_RDONLY);
	if (fd < 0)
	{
		std::cerr << "Error: cannot open file" << std::endl; 
		close(fd);
		return(1);
	}
	struct stat st;
	if(stat(av[1], &st) < 0)
	{
		std::cerr << "Error: unable to read file information" << std::endl;
		close(fd);
		return(1);
	}
	size_t size = st.st_size;
	std::vector<char>buffer(size + 1, '\0');
	ssize_t b_read = read(fd, &buffer[0], size);
	if(b_read < 0)
	{
		std::cerr << "Error: reading failed" << std::endl;
		close(fd);
		return(1);
	}
	close(fd);

	try
	{
		WebServer server(&buffer[0], size);
		server.exec();
	}
	catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

}






