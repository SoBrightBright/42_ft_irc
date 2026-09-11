#ifndef IRCHELPERS_HPP
# define IRCHELPERS_HPP

	# include <string>
	class Client;
	
	namespace IrcNumeric {
		// common (parser, registry)
		const std::string	ERR_UNKNOWNCOMMAND	= "421";
		const std::string	ERR_NOTREGISTERED	= "451";
		const std::string	ERR_NEEDMOREPARAMS	= "461";

		// NICK
		const std::string	ERR_NONICKNAMEGIVEN		= "431";
		const std::string	ERR_ERRONEUSNICKNAME	= "432";
		const std::string	ERR_ERRNICKNAMEINUSE	= "433";

		// USER, PASS
		const std::string	ERR_ALREADYREGISTERED	= "462";
		const std::string	ERR_PASSWDMISMATCH		= "464";

		// PING
		const std::string	ERR_NOORIGIN	= "409";
	}

	namespace IrcReply {
		std::string	targetname(const Client &client);
		std::string	formatReply(const std::string &servername, const std::string code,
								const std::string &target, const std::string &trailing);
		std::string	formatReplyWithParameter(const std::string &servername, const std::string code,
											 const std::string &target, const std::string &parameter,
											 const std::string &trailing);
		std::string	formatCommand(const std::string &servername, const std::string &command, const std::string &trailing);
	}

#endif