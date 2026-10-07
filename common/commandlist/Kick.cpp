#include "Kick.hpp"

#include "../../Client.hpp"
#include "../../network/Server.hpp"
#include "../../parse/Message.hpp"
#include "../../channel/Channel.hpp"
#include "../../common/IrcHelpers.hpp"

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
		client.sendReply(makeReply(Numeric::ERR_NEEDMOREPARAMS, target, "KICK :Not enough parameters"));
		return ;
	}

	Channel *channel = server.findChannel(params[0]);
	if (!channel)
	{
		client.sendReply(makeReply(Numeric::ERR_NOSUCHCHANNEL, target, params[0] + " :No such channel"));
		return ;
	}

	Client *targetClient = server.findClientByNickname(params[1]);
	if (!targetClient)
	{
		if (!channel->isMember(client))
			client.sendReply(makeReply(Numeric::ERR_NOTONCHANNEL, target, channel->getName() + " :You're not on that channel"));
		else if (!channel->isOperator(client))
			client.sendReply(makeReply(Numeric::ERR_CHANOPRIVSNEEDED, target, channel->getName() + " :You're not channel operator"));
		else		
			client.sendReply(makeReply(Numeric::ERR_USERNOTINCHANNEL, target, params[1] + " " + params[0] + " :They aren't on that channel"));
		return ;
	}

	std::string	comment = "";
	if (params.size() >= 3)
		comment = params[2];
	
	channel->handleKick(client, *targetClient, comment);
	if (channel->isEmpty())
		server.deleteChannel(params[0]);
}
