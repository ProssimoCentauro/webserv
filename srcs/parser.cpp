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

size_t Parser::parseServer()
{
    ServerConfig conf;
    if(token[pos].value == "server" && token[pos + 1].value == "{")
    {
        while(token[pos].value != "\0")
        {
            if(token[pos].value == "listen")
            {
                conf.listen_port = stringTo<int>(token[pos + 1].value);
            }
            else if(token[pos].value == "root")
            {
                conf.root = token[pos + 1].value;
            }
            else if(token[pos].value == "client_max_body_size")
            {
                conf.client_max_body_size = stringTo<int>(token[pos + 1].value);
            }
            else if(token[pos].value == "error_page")
            {
                conf.error_pages.insert(std::pair<int, std::string>(stringTo<int>(token[pos + 1].value), token[pos + 2].value));
                pos++;
            }
            if(token[pos].type == WORD && token[pos].value == "location")
            {
                pos = parseLocation(conf);
                continue;
            }
            pos++;
        }
    }
    config.addServer(conf);
    
    return(pos);
}

void Parser::parser()
{
    while(pos < token.size())
    {
        if(token[pos].type == WORD && token[pos].value == "server")
        {
            pos = parseServer();
            continue;
        }
        pos++;
    }
}

size_t Parser::parseLocation(ServerConfig& conf)
{
    LocationConfig loc;
    loc.client_max_body_size = conf.client_max_body_size;
    loc.autoindex = false;
    if(token[pos].type == WORD && token[pos].value == "location")
    {
        while(token[pos].value != "{")
        {
            if(token[pos].type == WORD && token[pos].value[0] == '/')
                loc.path = token[pos].value;
            pos++;
        }
        if(token[pos].value == "{")
        {
            while(token[pos].value != "}")
            {
                if(token[pos].value == "methods")
                {
                    pos++;
                    while(token[pos].value != ";")
                    {
                        loc.methods.push_back(token[pos].value);
                        pos++;
                    }
                }
                else if(token[pos].value == "root")
                {
                    pos++;
                    loc.root = token[pos].value;
                }
                else if(token[pos].value == "upload_dir")
                {
                    pos++;
                    loc.upload_enabled = true;
                    loc.upload_path = token[pos].value;
                }
                else if(token[pos].value == "client_max_body_size")
                {
                    pos++;
                    loc.client_max_body_size = stringTo<int>(token[pos].value);
                }
                else if(token[pos].value == "index")
                {
                    pos++;
                    loc.index = token[pos].value;
                }
                else if(token[pos].value == "autoindex")
                {
                    pos++;
                    if (token[pos].value == "on")
                        loc.autoindex = true;
                    else if (token[pos].value == "off")
                        loc.autoindex = false;
                }
                else if(token[pos].value == "cgi_extension")
                {
                    loc.cgi.insert(std::pair<std::string, std::string>(token[pos + 1].value, token[pos + 2].value));
                    pos++;
                }
                else if(token[pos].value == "return")
                {
                    loc.redirect.insert(std::pair<int, std::string>(stringTo<int>(token[pos + 1].value), token[pos + 2].value));
                    pos++;
                }
                pos++;
            }
        }
        conf.locations.push_back(loc);
    }
    return(pos);
}

void Parser::printConfig()
{
    const std::vector<ServerConfig>& c = config.getServers();
    for(size_t i = 0; i < c.size(); i++)
    {
        std::cout << "Server #" << i << std::endl;
        /*for(size_t j = 0; j < c[i].listen_port.size(); j++)
            std::cout << "Port: " << c[i].listen_port[j] << std::endl;*/
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
