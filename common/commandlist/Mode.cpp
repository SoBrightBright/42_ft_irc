#include "Mode.hpp"

#include "../../Client.hpp"
#include "../../network/Server.hpp"
#include "../../parse/Message.hpp"
#include "../../channel/Channel.hpp"
#include "../../common/IrcHelpers.hpp"

Mode::Mode() {}
Mode::~Mode() {}
Mode::Mode(const Mode &obj) { (void)obj; }
Mode	&Mode::operator=(const Mode &obj) { (void)obj; return *this; }

bool	Mode::needLogin() const { return true; }

// <channel> *( ( "-" / "+" ) *<modes> *<modeparams> )

void	Mode::execute(Server &server, Client &client, const Message &msg)
{
	const std::vector<std::string> &params = msg.getParameter();

	std::string	targetName = "*";
	if (client.isRegistered())
		targetName = client.getNickname();

	if (params.size() < 1)
	{
		client.getWriteBuffer() += makeReply(Numeric::ERR_NEEDMOREPARAMS, targetName, "MODE :Not enough parameters");
		return ;
	}

	Channel *channel = server.findChannel(params[0]);
	if (!channel)
	{
		client.getWriteBuffer() += makeReply(Numeric::ERR_NOSUCHCHANNEL, targetName, params[0] + " :No such channel");
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

	// 'o'는 server에 접근해야 하기 때문에 channel 클래스 안이 아닌 여기서 처리

	size_t paramIndex = 0;
	bool enable = true;
	std::string filteredModes;
	std::vector<std::string> filteredParams;
	char lastAppliedSign = 0;

	for (size_t i = 0; i < modes.size(); i++)
	{
		char c = modes[i];
		if (c == '+')
		{
			enable = true;
			continue ;
		}
		if (c == '-')
		{
			enable = false;
			continue ;
		}

		if (c == 'o')
		{
			if (paramIndex >= modeParams.size())
			{
				client.getWriteBuffer() += makeReply(Numeric::ERR_NEEDMOREPARAMS, targetName, "MODE :Not enough parameters");
				continue ;
			}
			std::string nickname = modeParams[paramIndex++];
			Client *target = server.findClientByNickname(nickname);
			if (target)
				channel->handleModeOperator(enable, *target, client);
			else
				client.getWriteBuffer() += makeReply(Numeric::ERR_USERNOTINCHANNEL, targetName, nickname + " " + channel->getName() + " :They aren't on that channel");
			continue ;
		}

		// 기능적으로 유의미한 부호만 저장

		if (c == 'i' || c == 't' || c == 'k' || c == 'l')
		{
			char sign = enable ? '+' : '-';
			if (sign != lastAppliedSign)
			{
				filteredModes += sign;
				lastAppliedSign = sign;
			}
			filteredModes += c;
	
			if (c == 'k' || c == 'l')
			{
				if (enable)
				{
					if (paramIndex >= modeParams.size())
					{
						client.getWriteBuffer() += makeReply(Numeric::ERR_NEEDMOREPARAMS, targetName, "MODE :Not enough parameters");
						continue ;
					}
					filteredParams.push_back(modeParams[paramIndex++]);
				}
			}
		}
		else
		{
			client.getWriteBuffer() += makeReply(Numeric::ERR_UNKNOWNMODE, targetName, std::string(1, c) + " :is unknown mode char to me for " + channel->getName());
		}
	}
	
	if (!filteredModes.empty())
		channel->handleMode(client, filteredModes, filteredParams);
}