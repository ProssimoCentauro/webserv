#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include "Request.hpp"
#include "Response.hpp"
#include "structures.h"

class RequestHandler
{
public:
    RequestHandler();
    RequestHandler(const RequestHandler& other);
    RequestHandler& operator=(const RequestHandler& other);
    ~RequestHandler();

    Response handle(
        const Request& request,
        const ServerConfig& server
    );
};

#endif

