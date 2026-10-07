#include "IrcHelpers.hpp"

#include "../Client.hpp"

#include <cctype>

std::string IrcReply::targetname(const Client &client) {
	if (client.isRegistered())
		return client.getNickname();
	return "*";
}

std::string	IrcReply::formatReply(const std::string code,
								  const std::string &target, const std::string &trailing)
{ return (":ircserv " + code + " " + target + " :" + trailing); }

std::string IrcReply::formatReplyWithParameter (const std::string code, const std::string &target,
												const std::string &parameter, const std::string &trailing)
{ return (":ircserv " + code + " " + target + " " + parameter + " :" + trailing); }

std::string	IrcReply::formatCommand(const std::string &command, const std::string &trailing)
{ return (":ircserv " + command + " :" + trailing); }

// RFC2812 2.2: {}|^는 []\~의 소문자 취급
static char	lowerChar(char c) {
	switch (c) {
		case '[':	return '{';
		case ']':	return '}';
		case '\\':	return '|';
		case '~':	return '^';
		default:	return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
	}
}

std::string	IrcText::toLower(const std::string &str) {
	std::string res = str;

	for (size_t idx = 0; idx < res.size(); ++idx)
		res[idx] = lowerChar(res[idx]);
	return res;
}
