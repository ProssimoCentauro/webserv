#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "Request.hpp"
#include "Response.hpp"
#include "structures.h"

class CGIHandler
{
public:
    CGIHandler();
    CGIHandler(const CGIHandler& other);
    CGIHandler& operator=(const CGIHandler& other);
    ~CGIHandler();

    Response execute(
        const Request& request,
        const LocationConfig& location
    );
};

#endif

