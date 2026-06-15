*This project has been created as part of the 42 curriculum by [lluciani](https://github.com/luys93), [rtodaro](https://github.com/ProssimoCentauro).*

## Description

**webserv** is a fully functional HTTP server implemented in C++ 98. The project aims to build a production-like web server from scratch, understanding the core concepts of HTTP protocol, socket programming, and server architecture.

### Goal
The primary objective is to create a server capable of:
- Handling HTTP/1.1 requests and responses
- Managing multiple connections concurrently
- Parsing HTTP requests correctly
- Serving static files
- Supporting configuration files for customizable behavior
- Implementing proper error handling and status codes

### Overview
This server demonstrates fundamental concepts in network programming and protocol implementation, without relying on external HTTP libraries. It implements the HTTP protocol specifications while maintaining clean, efficient C++ code following the C++ 98 standard.

## Instructions

### Compilation
```bash
make
```
The Makefile is configured to compile all necessary source files with appropriate flags for C++ 98 compliance.

### Installation
No external installation is required. Simply clone the repository and compile using the provided Makefile.

```bash
git clone https://github.com/ProssimoCentauro/webserv.git
cd webserv
make
```

### Execution
```bash
./webserv [configuration_file]
```

**Example:**
```bash
./webserv config.conf
```

### Configuration
The server can be configured via a configuration file (similar to Nginx syntax). Key configuration options typically include:
- Port number
- Server name
- Root directory for serving files
- Route-specific settings
- Error pages

## Resources

### HTTP Protocol & Web Servers
- [RFC 7230: HTTP/1.1 Message Syntax and Routing](https://tools.ietf.org/html/rfc7230)
- [RFC 7231: HTTP/1.1 Semantics and Content](https://tools.ietf.org/html/rfc7231)
- [MDN: HTTP Overview](https://developer.mozilla.org/en-US/docs/Web/HTTP)
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)

### C++ Socket Programming
- [C++ Reference Documentation](https://en.cppreference.com/)
- [Unix Socket API Reference](https://man7.org/linux/man-pages/man7/socket.7.html)
- [Socket Programming Tutorial](https://www.geeksforgeeks.org/socket-programming-cc/)

### Server Architecture
- [Understanding Nginx Architecture](https://www.nginx.com/blog/inside-nginx-how-we-designed-for-performance-scale/)
- [Event-Driven Programming](https://en.wikipedia.org/wiki/Event-driven_architecture)

### AI Usage
AI was utilized in this project as a technical reference tool for:
- **Clarification of complex technical concepts**: Understanding intricate aspects of HTTP protocol semantics, socket programming paradigms, and network I/O patterns
- **Component interaction analysis**: Gaining deeper insights into how different architectural components interact with one another, including request parsing pipelines, event handling mechanisms, and response generation workflows

The core implementation, architectural decisions, and problem-solving approach were developed independently. AI assistance was limited to conceptual clarification and technical explanation of existing patterns and best practices in server architecture.

## Features

- ✅ HTTP/1.1 Protocol Support
- ✅ Multi-client Connection Handling
- ✅ Static File Serving
- ✅ Configuration File Support
- ✅ Error Handling with Proper HTTP Status Codes
- ✅ Request Parsing and Validation
- ✅ C++ 98 Compliant Code

## Technical Choices

- **Language**: C++ 98 for compatibility and performance
- **Architecture**: Event-driven with socket-based I/O
- **No External Libraries**: Pure standard library implementation
- **Configuration**: File-based similar to industry standards (Nginx-like)

## Project Structure

```
webserv/
├── Makefile
├── src/
│   ├── main.cpp
│   ├── ClientConnection.cpp
│   ├── Config.cpp
│   ├── Lexer.cpp
│   ├── Parser.cpp
|   ├── Poller.cpp
|   ├── Request.cpp
|   ├── Response.cpp
|   └── WebServer.cpp
|
├── include/
|   └── [all class declarations]
|
├── www/
|   ├── cgu-bin/
|   ├── errors/
|   ├── files/
|   ├── images/
|   ├── protected/
|   ├── uploads/
|   ├── index.html
|   ├── script.js
|   ├── style.css
|   └── upload.html
|
├── config.conf
└── README.md
```

## License

This project is part of the 42 school curriculum. See the LICENSE file for more information if applicable.

## Testing

To test the server, use tools like:
```bash
curl http://localhost:8080
telnet localhost 8080
```

---
