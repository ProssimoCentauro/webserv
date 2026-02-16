#include "lexer.hpp"


Lexer::Lexer(char *input, size_t size): pos(0), size(size), input(input), line(0)
{

}

Lexer::~Lexer()
{

}

bool isWord(std::string &s)
{
	for(size_t i = 0; i <= s.size(); i++)
	{
		if(s[i] == '{' || s[i] == '}' || s[i] == ';')
			return(false);
	}
	return(true);
}

bool isNumber(std::string &s)
{
	for(size_t i = 0; i <= s.size(); i++)
	{
		if(!isdigit(s[i]))
			return(false);
	}
		return(true);
}

bool Lexer::isSymbol()
{
	if(input[pos] == '{' || input[pos] == '}' || input[pos] == ';')
		return(true);
	return(false);
}

bool Lexer::isPath()
{
	if((input[0] == '.' && input[0] == '/'))
		return(true);
	return(false);
}

bool Lexer::isSpace()
{
	if(input[pos] == ' ' || input[pos] == '\r' || input[pos] == '\t' || input[pos] == '\n')
		return(true);
	return(false);
}

bool Lexer::isToken()
{
	if(!isSpace() && !isSymbol())
		return(true);
	return(false);
}

void Lexer::lexSymbol()
{
	size_t start = pos;
	Token tok;
	while(pos < size && isSymbol() == true)
		pos++;
	tok.value = std::string(input + start, pos - start);
	tok.type = SYMBOL;
	tok.line = line++;
	token.push_back(tok);
}

/*void Lexer::lexPath()
{
	size_t start = pos;
	Token tok;	
	while(pos < size  && !isSpace() && !isSymbol())
	{
		//std::cout << "prima dell'incremento" << std::endl;
		pos++;
	}
	tok.value = std::string(input + start, pos - start);
	//std::cout << "dopo token value?" << std::endl;
	tok.type = PATH;
	tok.line = 3;
	token.push_back(tok);
}*/

void Lexer::takeToken()
{
	size_t start = pos;
	Token tok;
	
	while(pos < size && (!isSpace() && !isSymbol()))
	{
		pos++;
	}
	tok.value = std::string(input + start, pos - start);
	tok.type = NONE;
	tok.line = line++;
	/*if(start == pos)
		return;*/
	token.push_back(tok);
}

/*void Lexer::tokenizer()
{
	TokenRule rule[] = {
		{&Lexer::isNumber, &Lexer::lexNumber},
		{&Lexer::isWord, &Lexer::lexWord},
		{&Lexer::isSymbol, &Lexer::lexSymbol},
		{&Lexer::isPath, &Lexer::lexPath}
	};

	while(pos < size)
	{
		if(isSpace())
			pos++;
		bool is_matched = false;		
		for(int i = 0; i < 4; i++)
		{
			if(isSpace())
				pos++;
			if((this->*rule[i].condition)() == true)
			{
				(this->*rule[i].lexFunction)();
				is_matched = true;
				break;
			}
			if(i == 4 && !is_matched)
				pos++;
		}

	}
		
}*/

void Lexer::tokenizer()
{
	TokenRule rule[] = {
		{&Lexer::isToken, &Lexer::takeToken},
		{&Lexer::isSymbol, &Lexer::lexSymbol}
	};

	while(pos < size)
	{
		if(isSpace())
			pos++;
		bool is_matched = false;		
		for(int i = 0; i < 2; i++)
		{
			if(isSpace())
				pos++;
			if((this->*rule[i].condition)() == true)
			{
				(this->*rule[i].lexFunction)();
				is_matched = true;
				break;
			}
			if(i == 2 && !is_matched)
				pos++;
		}

	}
}
void Lexer::assignType()
{
	for(size_t i = 0; i < token.size(); i++)
	{
		if(token[i].type == NONE )
		{
			for(size_t j = 0; token[i].value[j]; j++)
			{
				if(token[i].value[0] == '.' && token[i].value[1] == '/')
					token[i].type = PATH;
				else if(isNumber(token[i].value) == true)
					token[i].type = NUMBER;
				else if(isWord(token[i].value) == true)
					token[i].type = WORD;
				else
				{
					i++;
					continue;
				}
			}
		}
	}
}

std::string printStr(TokenType n)
{
	if(n == 1)
		return("WORD");
	else if(n == 2)
		return("NUMBER");
	else if(n == 3)
		return("SYMBOL");
	else if(n == 4)
		return("PATH");
	return("NONE");
}

void Lexer::printTok()
{
    for (size_t i = 0; i < token.size(); i++)
    {
        std::cout << "VALUE: " << token[i].value << std::endl;
        std::cout << "TYPE: "  << printStr(token[i].type)  << std::endl;
        std::cout << "LINE: "  << token[i].line  << std::endl;
        std::cout << "----------------" << std::endl;
    }
}

const std::vector<Token>& Lexer::getToken() const
{
	return token;
}

