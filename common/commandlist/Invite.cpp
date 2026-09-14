#include "Invite.hpp"

#include "../../Client.hpp"
#include "../../network/Server.hpp"
#include "../../parse/Message.hpp"
#include "../../channel/Channel.hpp"
#include "../../common/IrcHelpers.hpp"

Invite::Invite() {}
Invite::~Invite() {}
Invite::Invite(const Invite &obj) { (void)obj; }
Invite	&Invite::operator=(const Invite &obj) { (void)obj; return *this; }

bool	Invite::needLogin() const { return true; }

// <nickname> <channel>

void	Invite::execute(Server &server, Client &client, const Message &msg)
{
	const std::vector<std::string> &params = msg.getParameter();

	std::string	target = "*";
	if (client.isRegistered())
		target = client.getNickname();

	if (params.size() < 2)
	{
		client.getWriteBuffer() += makeReply(Numeric::ERR_NEEDMOREPARAMS, target, "INVITE :Not enough parameters");
		return ;
	}

	Client *targetClient = server.findClientByNickname(params[0]);
	if (!targetClient)
	{
		client.getWriteBuffer() += makeReply(Numeric::ERR_NOSUCHNICK, target, params[0] + " :No such nick/channel");
		return ;
	}

	Channel *channel = server.findChannel(params[1]);
	if (!channel)
	{
		client.getWriteBuffer() += makeReply(Numeric::ERR_NOSUCHCHANNEL, target, params[1] + " :No such channel");
		return ;
	}

	channel->handleInvite(client, *targetClient);
}

// TODO:: "there is no requirement that the channel the target user is being invited to must exist or be a valid channel" -> 어케 처리할지 고민해 보기