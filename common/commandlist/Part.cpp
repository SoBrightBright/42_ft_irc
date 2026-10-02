#include "Part.hpp"

#include "../../Client.hpp"
#include "../../network/Server.hpp"
#include "../../parse/Message.hpp"
#include "../../channel/Channel.hpp"
#include "../../common/IrcHelpers.hpp"

Part::Part() {}
Part::~Part() {}
Part::Part(const Part &obj) { (void)obj; }
Part	&Part::operator=(const Part &obj) { (void)obj; return *this; }

bool	Part::needLogin() const { return true; }

// 10/02: 메세지 파라미터 변경으로 인한 코드 수정 & 함수 통일
void	Part::execute(Server &server, Client &client, const Message &msg)
{
	const std::vector<std::string> &params = msg.getParameter();

	if (params.size() < 1)
	{
		client.sendReply(makeReply(Numeric::ERR_NEEDMOREPARAMS, IrcReply::targetname(client), 
								"PART :Not enough parameters"));
		return ;
	}

	Channel *channel = server.findChannel(params[0]);
	if (!channel)
	{
		client.sendReply(makeReply(Numeric::ERR_NOSUCHCHANNEL, IrcReply::targetname(client),
								params[0] + " :No such channel"));
		return ;
	}

	std::string	comment = "";
	if (params.size() >= 2)
		comment = params[1];

	channel->handlePart(client, comment);
	if (channel->isEmpty())
		server.deleteChannel(params[0]);
}