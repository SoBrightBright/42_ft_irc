#include "CommandRegistry.hpp"

#include "../network/Server.hpp"
#include "../Client.hpp"
#include "Message.hpp"

#include "../common/commandlist/ICommand.hpp"

#include "../common/IrcHelpers.hpp"

CommandRegistry::CommandRegistry() {}
CommandRegistry::~CommandRegistry() {
	for (std::map<std::string, ICommand*>::iterator it = _commands.begin(); it != _commands.end(); ++it)
		delete it->second;
}
CommandRegistry::CommandRegistry(const CommandRegistry &obj) {(void)obj;}
CommandRegistry	&CommandRegistry::operator=(const CommandRegistry &obj) {(void)obj; return *this;}

void	CommandRegistry::registerCommand(const std::string &name, ICommand *command) {
	std::map<std::string, ICommand*>::const_iterator it = _commands.find(name);
	if (it != _commands.end())
		delete it->second;
	_commands[name] = command;
}

// /**/ 로 감싸진 부분은 실제 구현 함수가 없어서 우선 두었어요.
void	CommandRegistry::dispatch(Server &server, Client &client, const Message &message) const {
	std::map<std::string, ICommand*>::const_iterator it = _commands.find(message.getCommand());

	// cannot find command: error 421 
	if (it == _commands.end()) {
		// RFC - target = "*" (before login) OR nickname (after login)
		std::string	target = "*";
	

		if (client.isRegistered())
			target = client.getNickname();
		/*
			client.sendReply(IrcReply::formatReplyWithParam(server.getName(), IrcNumeric::ERR_UNKNOWNCOMMAND,
															target, message.getCommand(), "Unknown command"));
		*/
		return;
	}

	// no login with login command: error 451
	if (it->second->needLogin() && !client.isRegistered()) {
		/*
			client.sendReply(IrcReply::formatReply(server.getName(), IrcNumeric::ERR_NOTREGISTERED,
													"*", "You have not registered"));
		*/
		return;
	}

	it->second->execute(server, client, message);
}
