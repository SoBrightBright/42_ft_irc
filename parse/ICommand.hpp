#ifndef ICOMMAND_HPP
# define ICOMMAND_HPP

	class Server;
	class Client;
	class Message;

	class	ICommand {
		public:
			virtual ~ICommand() {}
	
			virtual bool	needLogin() const = 0;
			virtual void 	execute(Server &server, Client &client, const Message &msg) = 0;
	};

#endif