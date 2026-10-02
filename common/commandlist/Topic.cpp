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
// 10/02: 메세지 파라미터 변경으로 인한 코드 수정 & 함수 통일
void	Topic::execute(Server &server, Client &client, const Message &msg)
{
	const std::vector<std::string> &params = msg.getParameter();

	std::string	target = "*";
	if (client.isRegistered())
		target = client.getNickname();

	if (params.size() < 1)
	{
		client.sendReply(makeReply(Numeric::ERR_NEEDMOREPARAMS, target, "TOPIC :Not enough parameters"));
		return ;
	}

	Channel *channel = server.findChannel(params[0]);
	if (!channel)
	{
		client.sendReply(makeReply(Numeric::ERR_NOSUCHCHANNEL, target, params[0] + " :No such channel"));
		return ;
	}

	std::string topic = "";
	bool hastopicparam = false;
	if (params.size() >= 2)
	{
		topic = params[1];
		hastopicparam = true;
	}
	
	channel->handleTopic(client, topic, hastopicparam);
}