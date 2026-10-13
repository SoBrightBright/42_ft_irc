#ifndef COMMANDREGISTRY_HPP
# define COMMANDREGISTRY_HPP

	# include <string>
	# include <map>

	class	Server;
	class	Client;
	class	Message;
	class	ICommand;

	class	CommandRegistry {
		private:
			std::map<std::string, ICommand*>	_commands;
		
			CommandRegistry(const CommandRegistry &obj);
			CommandRegistry	&operator=(const CommandRegistry &obj);
		
		public:
			CommandRegistry();
			~CommandRegistry();

			void	registerCommand(const std::string &name, ICommand *command);	
			void	dispatch(Server &server, Client &client, const Message &message) const;
	};

#endif