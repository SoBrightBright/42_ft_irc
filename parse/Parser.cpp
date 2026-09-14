#include "Parser.hpp"

#include "../network/Server.hpp"
#include "../Client.hpp"
#include "Message.hpp"

#include "CommandRegistry.hpp"
#include "../common/commandlist/ICommand.hpp"
#include "../common/commandlist/Nick.hpp"
#include "../common/commandlist/Pass.hpp"
#include "../common/commandlist/Ping.hpp"
#include "../common/commandlist/Pong.hpp"
#include "../common/commandlist/Quit.hpp"
#include "../common/commandlist/User.hpp"

/*
	#include "commandlist/Invite.hpp"
	#include "commandlist/Join.hpp"
	#include "commandlist/Kick.hpp"
	#include "commandlist/Mode.hpp"
	#include "commandlist/Part.hpp"
	#include "commandlist/Privmsg.hpp"
	#include "commandlist/Topic.hpp"

	// channel용 함수 목록 추가 
*/

Parser::Parser() {
	_commands.registerCommand("NICK", new Nick());
	_commands.registerCommand("PASS", new Pass());
	_commands.registerCommand("PING", new Ping());
	_commands.registerCommand("PONG", new Pong());
	_commands.registerCommand("QUIT", new Quit());
	_commands.registerCommand("USER", new User());
	/*
		_commands.registerCommand("INVITE", new Invite());
		_commands.registerCommand("JOIN", new Join());
		_commands.registerCommand("KICK", new Kick());
		_commands.registerCommand("MODE", new Mode());
		_commands.registerCommand("PART", new Part());
		_commands.registerCommand("PRIVMSG", new Privmsg());
		_commands.registerCommand("TOPIC", new Topic());
	*/
}

Parser::~Parser() {}
Parser::Parser(const Parser &obj) { (void)obj; }
Parser	&Parser::operator=(const Parser &obj) { (void)obj; return *this;}

bool	Parser::operate(Server &server, Client &client, const std::string &raw) {
	Message	msg;

	if (!msg.spliter(raw))
		return false;
	_commands.dispatch(server,client, msg);
	return true;
}
