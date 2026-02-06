#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

enum TokenType
{
	NONE,
	WORD,
	NUMBER,
	SYMBOL,
	PATH,
};

struct Token
{
	std::string value;
	TokenType type;
	int line;
};


#endif
