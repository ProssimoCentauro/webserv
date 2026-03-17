#include <iostream>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main()
{
    int server_fd;
    int client_fd;
    sockaddr_in addr;
    char buffer[1024];

    // 1️⃣ socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("socket");
        return 1;
    }

    // 2️⃣ bind
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8080);

    if (bind(server_fd, (sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        return 1;
    }

    // 3️⃣ listen
    if (listen(server_fd, 10) < 0)
    {
        perror("listen");
        return 1;
    }

    std::cout << "Server in ascolto su http://localhost:8080\n";

    // 4️⃣ accept
    client_fd = accept(server_fd, NULL, NULL);
    if (client_fd < 0)
    {
        perror("accept");
        return 1;
    }

    // 5️⃣ recv (leggiamo la request, anche se non la usiamo)
    std::memset(buffer, 0, sizeof(buffer));
    recv(client_fd, buffer, sizeof(buffer), 0);

    // 6️⃣ risposta HTTP
    const char *response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Length: 13\r\n"
        "Content-Type: text/plain\r\n"
        "\r\n"
        "Hello Webserv";

    send(client_fd, response, std::strlen(response), 0);

    // 7️⃣ close
    close(client_fd);
    close(server_fd);

    return 0;
}
