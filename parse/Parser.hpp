#ifndef PARSER_HPP
# define PARSER_HPP

	#include "CommandRegistry.hpp"
	#include <string>

	class Server;
	class Client;
	class ICommand;

	class	Parser {
		private:
			CommandRegistry	_commands;

			Parser(const Parser &obj);
			Parser	&operator=(const Parser &obj);

		public:
			Parser();
			~Parser();

			void	operate(Server &server, Client &client, const std::string &raw);
	};

#endif