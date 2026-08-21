NAME = ircserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -MMD -MP

CHANNEL_DIR   = ./channel/
NETWORK_DIR  = ./network/
PARSE_DIR	= ./parse/

SRCS = main.cpp Client.cpp $(NETWORK_DIR)Server.cpp
OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.d)

$(NAME) : $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY : all clean fclean re

all : $(NAME)

clean :
	rm -f $(OBJS) $(DEPS)

fclean : clean
	rm -f $(NAME)

re : fclean all

-include $(DEPS)