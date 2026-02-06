#ifndef PARSER_HPP
#define PARSER_HPP

#include "Config.hpp"

class Parser
{
    private:
            size_t pos;
            std::vector<Token> token;
            Config config;


    public:
            Parser(const std::vector<Token> &token);
            ~Parser();


};


#endif