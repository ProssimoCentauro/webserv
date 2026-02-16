#ifndef PARSER_HPP
#define PARSER_HPP

#include "Config.hpp"
#include "token.hpp"
#include <sstream>
#include <iostream> //per test

class Parser
{
    private:
            size_t pos;
            std::vector<Token> token;
            Config config;


    public:
            Parser();
            Parser(const std::vector<Token> &token);
            ~Parser();
            size_t parseServer();
            size_t parseLocation(ServerConfig& conf);
            void parser();
            void printConfig(); // test


};

template<typename T>

T stringTo(const std::string &s)
{
        std::stringstream ss(s);
        T value;
        ss >> value;
        return(value);
}

#endif