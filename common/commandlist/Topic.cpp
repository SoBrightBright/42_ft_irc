#include "Topic.hpp"

#include "../../Client.hpp"
#include "../../network/Server.hpp"
#include "../../parse/Message.hpp"
#include "../../channel/Channel.hpp"
#include "../../common/IrcHelpers.hpp"

Topic::Topic() {}
Topic::~Topic() {}
Topic::Topic(const Topic &obj) { (void)obj; }
Topic	&Topic::operator=(const Topic &obj) { (void)obj; return *this; }

bool	Topic::needLogin() const { return true; }

// <channel> [ <topic> ]

void	Topic::execute(Server &server, Client &client, const Message &msg)
{
	const std::vector<std::string> &params = msg.getParameter();

	std::string	target = "*";
	if (client.isRegistered())
		target = client.getNickname();

	if (params.size() < 1)
	{
		client.getWriteBuffer() += makeReply(Numeric::ERR_NEEDMOREPARAMS, target, "TOPIC :Not enough parameters");
		return ;
	}

	Channel *channel = server.findChannel(params[0]);
	if (!channel)
	{
		client.getWriteBuffer() += makeReply(Numeric::ERR_NOSUCHCHANNEL, target, params[0] + " :No such channel");
		return ;
	}

	std::string topic = "";
	bool hastopicparam = false;
	if (msg.hasTrailing())
	{
		topic = msg.getTrailing();
		hastopicparam = true;
	}
	
	channel->handleTopic(client, topic, hastopicparam);
}