#include "network/Server.hpp"
#include "Client.hpp"
#include "Reply.hpp"
#include <sstream>

// RFC 2812 2.3.1

std::string	makeReply(int code, const std::string &nick, const std::string &rest)
{
	std::string prefix = "ircserv";

	std::ostringstream cmd;
	// '0' padding
	if (code < 100)
		cmd << '0';
	if (code < 10)
		cmd << '0';
	cmd << code;

	// example: ":server 482 nick #channel :You're not channel operator"
	return ":" + prefix + " " + cmd.str() + " " + nick + " " + rest;
}

std::string makeCommand(const Client &client, const std::string &cmd, const std::string &rest)
{
	std::string prefix = client.getNickname() + "!" + client.getUsername() + "@" + client.getIp();
	
	// example: ":nick!user@host PART #channel :reason"
	if (!rest.empty())
		return ":" + prefix + " " + cmd + " " + rest;
	else
		return ":" + prefix + " " + cmd;
}

void	handlePersonalPrivmsg(Client &sender, Client &recipient, const std::string &message)
{
	recipient.getWriteBuffer() += makeCommand(sender, "PRIVMSG", recipient.getNickname() + " :" + message);
}