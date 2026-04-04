#ifndef LEXER_HPP
#define LEXER_HPP

#include "token.hpp"
#include <vector>
#include <iostream>

class Lexer
{
	private:
		size_t  pos;
		size_t size;
		const char *input;
		void lexSymbol();
		void takeToken();
		std::vector<Token> token;
	public:
		int line;
		Lexer(char *input, size_t size);
		Lexer(const Lexer& other);
		Lexer& operator=(const Lexer& other);
		~Lexer();
		bool isSymbol();
		bool isPath();
		bool isSpace();
		bool isToken();
		void printTok();
		void tokenizer();
		void assignType();
		
		
        const std::vector<Token>& getToken() const; // per test

	/*	class ConfigException : public std::exception
		{
			private:
					std::string str;
			public:
				ConfigException(std::string str);
				virtual ~ConfigException() throw();
				virtual const char* what() const throw();
		};				*/



};

struct TokenRule
{
        bool (Lexer::*condition)();
        void (Lexer::*lexFunction)();
};

#endif
