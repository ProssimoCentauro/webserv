#include "parser.hpp"

Parser::Parser(): pos(0),config()
{

}

Parser::Parser(const std::vector<Token> &token): pos(0), token(token),config()
{

}

Parser::~Parser()
{
    
}

Parser::ConfigException::ConfigException(std::string str): str(str)
{
  
}

const char* Parser::ConfigException::what() const throw()
{
	return(str.c_str());
}

Parser::ConfigException::~ConfigException() throw()
{

}

bool Parser::isNumber(std::string &s)
{
	for(size_t i = 0; i < s.size(); i++)
	{
		if(!isdigit(s[i]))
			return(false);
	}
		return(true);
}

std::string intTostr(int n)
{
    std::ostringstream os;
    os << n;
    return(os.str());
}

std::string& Parser::checkPos(size_t n)
{
    if(pos + n >= token.size())
        throw ConfigException("Unexpected end of file: " + intTostr(token[pos].line));
    return(token[pos + n].value);
}

void Parser::movePos(size_t n)
{
    if(pos + n >= token.size())
        throw ConfigException("Unexpected end of file" + intTostr(token[pos].line));
    pos += n;
    if(token[pos].value != ";")
        throw ConfigException("Expected ; after directive at line: " + intTostr(token[pos].line));
    else
        pos++; // skip ;
}

bool Parser::searchValue(std::string str, size_t pos)
{
    if(checkPos(pos) !=  str)
        return(false);
    return(true);
}


bool Parser::isMatch(std::string str)
{
    if(pos >= token.size())
        return(false);
    return(token[pos].value == str);
}

std::string Parser::incresePos(std::string str, size_t npos)
{
    if(!isMatch(str))
        throw ConfigException("Unexpected token: " + str + " at line " + intTostr(token[pos].line));
    if(pos + npos >= token.size())
        throw ConfigException("out of range at line: " + intTostr(token[pos].line));
    pos += npos;
    return(token[pos].value);
}


void Parser::validateLocation(LocationConfig& loc)
{
    if(validateMethods(loc) == false)
                throw ConfigException("Invalid methods directive ");
    if(!loc.root.empty() && (loc.root[0] != '.' || loc.root[1] != '/'))
                throw ConfigException("Invalid root directive");
}

bool Parser::validateMethods(LocationConfig& loc)
{
    for(size_t i = 0; i < loc.methods.size(); i++)
    {
        if(loc.methods[i] != "GET" && loc.methods[i] != "POST" && loc.methods[i] != "DELETE")
            return(false);
    }
    return(true);
}

std::string Parser::giveToken()
{
    if(pos >= token.size())
        throw ConfigException("Unexpected end of file: " + intTostr(token[pos].line));
    return(token[pos].value);
}

bool Parser::check_brackets()
{
    int brackets = 0;
    for(size_t i = 0; i < token.size(); i++)
    {
        if(token[i].value == "{")
            brackets++;
        else if(token[i].value == "}")
            brackets--;
    }
    if(brackets > 0)
        throw ConfigException("Expected { before end of file: " + intTostr(token[pos].line)); 
    if(brackets < 0)
        throw ConfigException("invalid brackets annidation at line: "+ intTostr(token[pos].line));
    else
        return(true);
}

void Parser::parser()
{
    if(check_brackets() == false)
        return;
    while(pos < token.size())
    {
            parseServer();
    }
}

void Parser::parseServer()
{
    incresePos("server", 1);
    incresePos("{", 1);
    if(isMatch("}"))
        throw ConfigException("Invalid sintax in server at line:" + intTostr(token[pos].line));

    ServerConfig conf;
    while(!isMatch("}"))
    {
        if(isMatch("server_name"))
        {
            conf.server_name = incresePos("server_name", 1);
            movePos(1);
        }
        else if(isMatch("listen"))
        {
            conf.listen_port = stringTo<int>(incresePos("listen", 1));
            movePos(1);
        }
        else if(isMatch("client_max_body_size"))
        {
            if(!isNumber(token[pos + 1].value))
                throw ConfigException("Error: invalid number at line: " + intTostr(token[pos].line));
            conf.client_max_body_size = stringTo<int>(incresePos("client_max_body_size", 1));
            movePos(1);
        }
        else if(isMatch("root"))
        {
            conf.root = (incresePos("root", 1));
            movePos(1);
        }
        else if(isMatch("error_page"))
        {
            conf.error_pages.insert(std::pair<int, std::string>(stringTo<int>(incresePos("error_page", 1)), checkPos(2))); 
            movePos(2);
        }
        else if(isMatch("location"))
        {
            parseLocation(conf);
            continue;
        }
        else
            throw ConfigException("Invalid directive in line: " + intTostr(token[pos].line));
    }
    config.addServer(conf);
    if(isMatch("}") && pos < token.size())
    {
        pos++;
        return;
    }
}

void Parser::parseLocation(ServerConfig& conf)
{
    LocationConfig loc;
    loc.client_max_body_size = conf.client_max_body_size;
    loc.autoindex = false;

    if(searchValue("location", 0))
    {
        incresePos("location", 1);
        if(token[pos].type != WORD || token[pos].value[0] != '/')
            throw ConfigException("Expected a path in line: " + intTostr(token[pos].line));
        loc.path = giveToken();
        incresePos(token[pos].value, 1);
    }
    incresePos("{", 1);
    while(!isMatch("}"))
    {
        if(isMatch("methods"))
        {
            incresePos("methods", 1);
            while(!isMatch(";"))
            {
                loc.methods.push_back(giveToken());
                pos++;
            }
            movePos(0);
        }
        else if(isMatch("root"))
        {
            loc.root = incresePos("root", 1);
            movePos(1);
        }
        else if(isMatch("upload_dir"))
        {
            loc.upload_enabled = true;
            loc.upload_path = incresePos("upload_dir", 1);
            movePos(1);
            
        }
        else if(isMatch("client_max_body_size"))
        {
            loc.client_max_body_size = stringTo<int>(incresePos("client_max_body_size", 1));
            movePos(1);
        }
        else if(isMatch("index"))
        {
            loc.index = incresePos("index", 1);
            movePos(1);
        }
        else if(isMatch("autoindex"))
        {
            incresePos("autoindex", 1);
            if (isMatch("on"))
                loc.autoindex = true;
            else if(isMatch("off"))
                loc.autoindex = false;
            else
                throw ConfigException("Invalid autoindex directive at line: " + intTostr(token[pos].line));
            movePos(1);
        }
        else if(isMatch("cgi_extension"))
        {
            loc.cgi.insert(std::pair<std::string, std::string>(incresePos("cgi_extension", 1), checkPos(2)));
            movePos(2);
        }
        else if(isMatch("return"))
        {
            loc.redirect.insert(std::pair<int, std::string>(stringTo<int>(incresePos("return", 1)), checkPos(2)));
            movePos(2);
        }
        else if(isMatch(";"))
            throw ConfigException("Invalid ; at the end the directive at line: " + intTostr(token[pos].line));
        else
            throw ConfigException("Invalid directive in line: " + intTostr(token[pos].line));
    }
    validateLocation(loc);
    conf.locations.push_back(loc);
    if(searchValue("}", 0))
        incresePos("}", 1);
    else
        throw ConfigException("Expected token: " + token[pos].value + " at line: " + intTostr(token[pos].line));
}

void Parser::printConfig()
{
    const std::vector<ServerConfig>& c = config.getServers();
    for(size_t i = 0; i < c.size(); i++)
    {
        std::cout << "Server #" << i << std::endl;
        std::cout << "Server_name: " << c[i].server_name << std::endl;
        std::cout << "Listen_Port: " << c[i].listen_port << std::endl;
        std::cout << "Root: " << c[i].root << std::endl;
        std::cout << "Client max body size: " << c[i].client_max_body_size << std::endl;

        for (std::map<int, std::string>::const_iterator it = c[i].error_pages.begin();
            it != c[i].error_pages.end(); ++it) 
        {
            std::cout << "Error " << it->first << " -> " << it->second << std::endl;
        }
        for (size_t j = 0; j < c[i].locations.size(); j++)
        {
            std::cout << std::endl << ":::::::::::::::::::::::" << std::endl;
            std::cout << "Path :" << c[i].locations[j].path << std::endl;
            std::cout << "Methods :";
            for(size_t k = 0; k < c[i].locations[j].methods.size(); ++k)
            {
                std::cout << c[i].locations[j].methods[k] << " ";
            } 
            std::cout << std::endl;
            std::cout << "root :" << c[i].locations[j].root << std::endl;
            std::cout << "upload_dir :" << c[i].locations[j].upload_path << std::endl;
            std::cout << "client_max_body_size :" << c[i].locations[j].client_max_body_size << std::endl;
            std::cout << "index :" << c[i].locations[j].index << std::endl;
            if(c[i].locations[j].autoindex == true)
                std::cout << "autoindex : " << "true" << std::endl;
            else
                std::cout << "autoindex : " << "false" << std::endl;
            for(std::map<std::string, std::string>::const_iterator it = c[i].locations[j].cgi.begin(); it != c[i].locations[j].cgi.end(); ++it)
            {
                std::cout << "cgi :" << it->first << "->" << it->second << std::endl;
            }
            for(std::map<int, std::string>::const_iterator it = c[i].locations[j].redirect.begin(); it != c[i].locations[j].redirect.end(); ++it)
            {
                std::cout << "redirection : " << it->first << " -> " << it->second << std::endl;
            }
            std::cout << ":::::::::::::::::::::::" << std::endl;
         }
    }
}
