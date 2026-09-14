#include "Privmsg.hpp"

#include "../network/Server.hpp"
#include "../Client.hpp"
#include "../parse/Message.hpp"
#include "../common/IrcHelpers.hpp" // TODO: 폴더 구조 맞추고 수정할 것

#include "../channel_ch/Channel.hpp" // TODO: 수정할 것

Privmsg::Privmsg() {}
Privmsg::~Privmsg() {}
Privmsg::Privmsg(const Privmsg &obj) { (void)obj; }
Privmsg	&Privmsg::operator=(const Privmsg &obj) { (void)obj; return *this; }

bool	Privmsg::needLogin() const { return true; }

// <msgtarget> <text to be sent>

void	Privmsg::execute(Server &server, Client &client, const Message &msg)
{
	const std::vector<std::string> &params = msg.getParameter();

	std::string	target = "*";
	if (client.isRegistered())
		target = client.getNickname();

	if (params.size() < 1)
	{
		client.getWriteBuffer() += makeReply(Numeric::ERR_NORECIPIENT, target, ":No recipient given (PRIVMSG)");
		return ;
	}

	Channel *channel = server.findChannel(params[0]);
	std::string comment = "";
	if (msg.hasTrailing())
		comment = msg.getTrailing();
	if (!channel)
	{
		Client *recipient = server.findClientByNickname(params[0]);
		if (!recipient)
		{
			client.getWriteBuffer() += makeReply(Numeric::ERR_NOSUCHNICK, target, params[0] + " :No such nick/channel");
			return ;			
		}
		handlePersonalPrivmsg(client, *recipient, comment);
		return ;
	}
	channel->handleChannelPrivmsg(client, comment);
}