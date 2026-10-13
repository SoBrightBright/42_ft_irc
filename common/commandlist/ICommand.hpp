#ifndef ICOMMAND_HPP
# define ICOMMAND_HPP

	class Server;
	class Client;
	class Message;

	class	ICommand {
		public:
			virtual ~ICommand() {}
			virtual bool	needLogin() const = 0;

			// execute 공통: parameter overcontain에 대해서 - 접근조차 없이 무시한다.
			virtual void 	execute(Server &server, Client &client, const Message &msg) = 0;
	};

#endif