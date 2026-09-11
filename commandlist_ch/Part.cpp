#include "Part.hpp"

#include "../network/Server.hpp"
#include "../Client.hpp"
#include "../parse/Message.hpp"
#include "../common/IrcHelpers.hpp" // TODO: 폴더 구조 맞추고 수정할 것

#include "../channel/Channel.hpp"

Part::Part() {}
Part::~Part() {}
Part::Part(const Part &obj) { (void)obj; }
Part	&Part::operator=(const Part &obj) { (void)obj; return *this; }

bool	Part::needLogin() const { return true; }

void	Part::execute(Server &server, Client &client, const Message &msg)
{
	const std::vector<std::string> &params = msg.getParameter();
	std::string	target = "*";
	if (client.isRegistered())
		target = client.getNickname();

	if (params.size() < 1)
	{
		client.getWriteBuffer() += makeReply(Numeric::ERR_NEEDMOREPARAMS, target, "PART :Not enough parameters");
		return ;
	}

	Channel *channel = server.findChannel(params[0]);
	if (!channel)
	{
		client.getWriteBuffer() += makeReply(Numeric::ERR_NOSUCHCHANNEL, target, params[0] + " :No such channel");
		return ;
	}

	std::string	comment = "";
	if (msg.hasTrailing())
		comment = msg.getTrailing();

	channel->handlePart(client, comment);
}