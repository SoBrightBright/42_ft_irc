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

			// ICommand가 Interface라서 pointer로 할당.
			void	registerCommand(const std::string &name, ICommand *command);	
			void	dispatch(Server &server, Client &client, const Message &message) const;
			// const 멤버함수는 멤버 변수 자체(포인터값)를 못바꾸게 막기는 하지만
			// 포인터가 가리키는 객체의 내용까지는 안 막아서...? execute가 뭔가 내부 상태를 바꿔도 괜찮다...(뭐라고요)
	};

#endif