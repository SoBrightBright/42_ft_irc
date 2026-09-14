#include "IrcHelpers.hpp"

#include "../Client.hpp"

std::string IrcReply::targetname(const Client &client) {
	if (client.isRegistered())
		return client.getNickname();
	return "*";
}

std::string	IrcReply::formatReply(const std::string code,
								  const std::string &target, const std::string &trailing)
{ return (":ircserv " + code + " " + target + " :" + trailing + "\r\n"); }

std::string IrcReply::formatReplyWithParameter (const std::string code, const std::string &target,
												const std::string &parameter, const std::string &trailing)
{ return (":ircserv " + code + " " + target + " " + parameter + " :" + trailing + "\r\n"); }

std::string	IrcReply::formatCommand(const std::string &command, const std::string &trailing)
{ return (":ircserv " + command + " :" + trailing + "\r\n"); }
