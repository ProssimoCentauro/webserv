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
		//void lexNumber();
		void lexSymbol();
		//void lexPath();
		void takeToken();
		std::vector<Token> token;
	public:
		int line;
		Lexer(char *input, size_t size);
		~Lexer();
		bool isSymbol();
		bool isPath();
		bool isSpace();
		bool isToken();
		void printTok();
		void tokenizer();
		void assignType();
		
		
        const std::vector<Token>& getToken() const; // per test
				



};

struct TokenRule
{
        bool (Lexer::*condition)();
        void (Lexer::*lexFunction)();
};

#endif
