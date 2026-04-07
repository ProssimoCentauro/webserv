NAME = webserv

SRCSF = ./srcs/
INCLUDESF = ./includes

SRCS =  $(SRCSF)Config.cpp $(SRCSF)Lexer.cpp $(SRCSF)Parser.cpp $(SRCSF)WebServer.cpp \
    $(SRCSF)ClientConnection.cpp $(SRCSF)Poller.cpp $(SRCSF)Request.cpp $(SRCSF)mainConfig.cpp $(SRCSF)Response.cpp\

OBJS = $(SRCS:.cpp=.o)

CC = c++
CPPFLAGS = -Wall -Wextra -Werror -std=c++98 -g
IFLAGS = -I $(INCLUDESF)
RM = rm -f

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CPPFLAGS) $(IFLAGS) -o $(NAME) $(OBJS) 

%.o: %.cpp
	$(CC) $(CPPFLAGS) $(IFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
