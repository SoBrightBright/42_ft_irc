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

static bool	isValidChannelName(const std::string &name)
{
	if (name.empty() || name.size() > 50)
		return false;
	if (name[0] != '#' && name[0] != '&')
		return false;
	if (name.find(' ') != std::string::npos)
		return false;
	if (name.find(',') != std::string::npos)
		return false;
	if (name.find('\x07') != std::string::npos)
		return false;
	return true;
}

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

	// JOIN 0 시 모든 채널에서 나가기
	if (params[0] == "0")
	{
		const std::map<std::string, Channel*> joined = client.getJoinedChannels();
		for (std::map<std::string, Channel*>::const_iterator it = joined.begin(); it != joined.end(); ++it)
		{
			it->second->handlePart(client, "");
			if (it->second->isEmpty())
				server.deleteChannel(it->first);
		}	
		return ;
	}

	if (!isValidChannelName(params[0]))
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
