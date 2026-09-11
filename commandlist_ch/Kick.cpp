#include "Kick.hpp"

#include "../network/Server.hpp"
#include "../Client.hpp"
#include "../parse/Message.hpp"
#include "../common/IrcHelpers.hpp" // TODO: 폴더 구조 맞추고 수정할 것

#include "../channel/Channel.hpp"

Kick::Kick() {}
Kick::~Kick() {}
Kick::Kick(const Kick &obj) { (void)obj; }
Kick	&Kick::operator=(const Kick &obj) { (void)obj; return *this; }

bool	Kick::needLogin() const { return true; }

// <channel> *( "," <channel> ) <user> *( "," <user> ) [<comment>]

void	Kick::execute(Server &server, Client &client, const Message &msg)
{
	const std::vector<std::string> &params = msg.getParameter();

	std::string	target = "*";
	if (client.isRegistered())
		target = client.getNickname();

	if (params.size() < 2)
	{
		client.getWriteBuffer() += makeReply(Numeric::ERR_NEEDMOREPARAMS, target, "KICK :Not enough parameters");
		return ;
	}

	Channel *channel = server.findChannel(params[0]);
	if (!channel)
	{
		client.getWriteBuffer() += makeReply(Numeric::ERR_NOSUCHCHANNEL, target, params[0] + " :No such channel");
		return ;
	}

	Client *targetClient = server.findClientByNickname(params[1]);
	if (!targetClient)
	{
		client.getWriteBuffer() += makeReply(Numeric::ERR_USERNOTINCHANNEL, target, params[1] + " " + params[0] + " :They aren't on that channel");
		return ;
	}

	std::string	comment = "";
	if (msg.hasTrailing())
		comment = msg.getTrailing();
	
	channel->handleKick(client, *targetClient, comment);
}
