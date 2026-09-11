#ifndef REPLY_HPP
#define REPLY_HPP

#include <string>

namespace Numeric
{
	// Channel
	const int	RPL_CHANNELMODEIS = 324;
	const int	RPL_TOPIC = 332;
	const int	RPL_NOTOPIC = 331;
	const int	RPL_NAMREPLY = 353;
	const int	RPL_ENDOFNAMES = 366;
	const int	RPL_INVITING = 341;
	const int	ERR_NOSUCHCHANNEL = 403;
	const int	ERR_CANNOTSENDTOCHAN = 404;
	const int	ERR_NOTONCHANNEL = 442;
	const int	ERR_USERNOTINCHANNEL = 441;
	const int	ERR_USERONCHANNEL = 443;
	const int	ERR_KEYSET = 467;
	const int	ERR_INVITEONLYCHAN = 473;
	const int	ERR_BADCHANNELKEY = 475;
	const int	ERR_CHANNELISFULL = 471;
	const int	ERR_UNKNOWNMODE = 472;
	const int	ERR_CHANOPRIVSNEEDED = 482;
};

std::string		makeReply(int code, const std::string &nick, const std::string &text);
std::string		makeCommand(const Client &client, const std::string &cmd, const std::string &rest);

#endif