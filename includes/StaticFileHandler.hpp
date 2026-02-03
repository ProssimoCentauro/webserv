#ifndef STATICFILEHANDLER_HPP
#define STATICFILEHANDLER_HPP

#include <string>
#include "Response.hpp"
#include "structures.h"

class StaticFileHandler
{
public:
    StaticFileHandler();
    StaticFileHandler(const StaticFileHandler& other);
    StaticFileHandler& operator=(const StaticFileHandler& other);
    ~StaticFileHandler();

    Response serve(
        const std::string& path,
        const LocationConfig& location
    );
};

#endif

