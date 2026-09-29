#include "Join.hpp"

#include "../../Client.hpp"
#include "../../network/Server.hpp"
#include "../../parse/Message.hpp"
#include "../../channel/Channel.hpp"
#include "../../common/IrcHelpers.hpp"

Join::Join() {}
Join::~Join() {}
Join::Join(const Join &obj) { (void)obj; }
Join	&Join::operator=(const Join &obj) { (void)obj; return *this; }

bool	Join::needLogin() const { return true; }

// ( <channel> *( "," <channel> ) [ <key> *( "," <key> ) ] ) / "0"

void	Join::execute(Server &server, Client &client, const Message &msg)
{
	const std::vector<std::string> &params = msg.getParameter();
	std::string	target = "*";
	if (client.isRegistered())
		target = client.getNickname();
	
	if (params.size() < 1)
	{
		client.sendReply(makeReply(Numeric::ERR_NEEDMOREPARAMS, target, "JOIN :Not enough parameters"));
		return ;
	}

	if (params[0] == "0")
	{
		const std::map<std::string, Channel*> joined = client.getJoinedChannels();
		for (std::map<std::string, Channel*>::const_iterator it = joined.begin(); it != joined.end(); ++it)
			it->second->handlePart(client, "");
		return ;
	}

	// 채널 이름 형식 검증
	if (params[0].empty() || (params[0][0] != '#' && params[0][0] != '&'))
	{
		client.sendReply(makeReply(Numeric::ERR_NOSUCHCHANNEL, target, params[0] + " :No such channel"));
		return ;
	}
	
	Channel *channel = server.findOrCreateChannel(params[0]);
	if (!channel)
		return;
	
	std::string key = "";
	if (params.size() > 1)
		key = params[1];

	channel->handleJoin(client, key);
}

// TODO: 멀티채널JOIN이 필요할까?