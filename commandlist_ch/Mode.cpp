#include "Mode.hpp"

#include "../network/Server.hpp"
#include "../Client.hpp"
#include "../parse/Message.hpp"
#include "../common/IrcHelpers.hpp" // TODO: 폴더 구조 맞추고 수정할 것

#include "../channel/Channel.hpp"

Mode::Mode() {}
Mode::~Mode() {}
Mode::Mode(const Mode &obj) { (void)obj; }
Mode	&Mode::operator=(const Mode &obj) { (void)obj; return *this; }

bool	Mode::needLogin() const { return true; }

// <channel> *( ( "-" / "+" ) *<modes> *<modeparams> )

void	Mode::execute(Server &server, Client &client, const Message &msg)
{
	const std::vector<std::string> &params = msg.getParameter();

	std::string	target = "*";
	if (client.isRegistered())
		target = client.getNickname();

	if (params.size() < 1)
	{
		client.getWriteBuffer() += makeReply(Numeric::ERR_NEEDMOREPARAMS, target, "MODE :Not enough parameters");
		return ;
	}

	Channel *channel = server.findChannel(params[0]);
	if (!channel)
	{
		client.getWriteBuffer() += makeReply(Numeric::ERR_NOSUCHCHANNEL, target, params[0] + " :No such channel");
		return ;
	}

	std::string modes = "";
	std::vector<std::string> modeParams;

	if (params.size() > 1)
	{
		modes = params[1];
		for (size_t i = 2; i < params.size(); i++)
			modeParams.push_back(params[i]);
	}
	
	channel->handleMode(client, modes, modeParams);
}