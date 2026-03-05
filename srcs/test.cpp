#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

int	main()
{
	int fd;
	struct sockaddr_in addr;
	int addrlen;
	int connection;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(8080);
	addr.sin_addr.s_addr = INADDR_ANY;

	fd = socket(AF_INET, SOCK_STREAM, 0);

	addrlen = sizeof(addr);
	
	if (fd < 0)
	{
		std::cerr << "socket syscall failed" << std::endl;
		exit(1);
	}
	std::cout << "socket fd: " << fd << std::endl;

	if (bind(fd, (const struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		std::cerr << "bind syscall failed" << std::endl;
		exit(1);
	}
	std::cout << "fd binded succesfully!" << std::endl;

	if (listen(fd, 10) < 0)
	{
		std::cerr << "listen syscall failed" << std::endl;
		exit(1);
	}
	std::cout << "fd is listening..." << std::endl;
	
	if (connection = accept(fd, (struct sockaddr *)&addr, (socklen_t *)&addrlen) < 0)
	{
		std::cerr << "accept syscall failed" << std::endl;
		exit(1);
	}
	std::cout << "fd accepted!" << std::endl;


	char buffer[100];
	read(connection, buffer, 100);
	std::cout << "received message: " << buffer << std::endl;
	
	send(connection, "good talk, bye!", 15, 0);

	close(connection);
	close(fd);

	return 0;
}
