NAME = ircserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -MMD -MP

CHANNEL_DIR   = ./channel/
NETWORK_DIR  = ./network/
PARSE_DIR	= ./parse/
COMMON_DIR	= ./common/
COMMAND_DIR	= ./common/commandlist/

SRCS = main.cpp Client.cpp $(NETWORK_DIR)Server.cpp \
	$(PARSE_DIR)Message.cpp $(PARSE_DIR)CommandRegistry.cpp $(PARSE_DIR)Parser.cpp \
	$(COMMON_DIR)IrcHelpers.cpp \ $(COMMON_DIR)Reply.cpp \
	$(COMMAND_DIR)Nick.cpp $(COMMAND_DIR)Pass.cpp $(COMMAND_DIR)Ping.cpp \
	$(COMMAND_DIR)Pong.cpp $(COMMAND_DIR)Quit.cpp $(COMMAND_DIR)User.cpp \
	$(COMMAND_DIR)Invite.cpp $(COMMAND_DIR)Join.cpp $(COMMAND_DIR)Kick.cpp \
	$(COMMAND_DIR)Mode.cpp $(COMMAND_DIR)Part.cpp $(COMMAND_DIR)Privmsg.cpp \
	$(COMMAND_DIR)Topic.cpp

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
