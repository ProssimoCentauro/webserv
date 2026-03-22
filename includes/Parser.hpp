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
            //size_t parseServer();
            //size_t parseLocation(ServerConfig& conf);
            //void parser();
            void printConfig(); // test
            bool isNumber(std::string &s);

            std::string& checkPos(size_t n);
            bool searchValue(std::string str, size_t pos);
            void movePos(size_t n);
            bool isMatch(std::string str);
            std::string incresePos(std::string str, size_t npos);
            std::string giveToken();
            void parser();
            void parseServer();
            void parseLocation(ServerConfig& conf);
            bool check_brackets();
            bool validateMethods(LocationConfig& loc);
            void validateLocation(LocationConfig& loc);
            //std::string assignAndIncrese(size_t n);
			
			Config& getConfig();

		class ConfigException : public std::exception
		{
			private:
					std::string str;
			public:
				ConfigException(std::string str);
				virtual ~ConfigException() throw();
				virtual const char* what() const throw();
		};	


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
