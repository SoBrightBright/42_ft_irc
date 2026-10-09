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
// 10/02: 메세지 파라미터 변경으로 인한 코드 수정 & 함수 통일
void	Mode::execute(Server &server, Client &client, const Message &msg)
{
	const std::vector<std::string> &params = msg.getParameter();

	std::string	targetName = "*";
	if (client.isRegistered())
		targetName = client.getNickname();

	if (params.size() < 1 || params[0].empty())
	{
		client.sendReply(makeReply(Numeric::ERR_NEEDMOREPARAMS, targetName, "MODE :Not enough parameters"));
		return ;
	}

	// irssi는 접속하자마자 사용자 모드 명령을 자동으로 보내는데, 우리 과제는 사용자 모드를 지원하지 않으므로 조용히 무시
	if (params[0][0] != '#' && params[0][0] != '&')
		return ;
	
	Channel *channel = server.findChannel(params[0]);
	if (!channel)
	{
		client.sendReply(makeReply(Numeric::ERR_NOSUCHCHANNEL, targetName, params[0] + " :No such channel"));
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

	if (modes.empty())
	{
		channel->handleMode(client, modes, modeParams);
		return ;
	}

	if (!channel->isOperator(client))
	{
		client.sendReply(makeReply(Numeric::ERR_CHANOPRIVSNEEDED, targetName, channel->getName() + " :You're not channel operator"));
		return ;
	}

	if (!modes.empty() && modes[0] != '+' && modes[0] != '-')
	{
		client.sendReply(makeReply(Numeric::ERR_UNKNOWNMODE, targetName, std::string(1, modes[0]) + " :is unknown mode char to me for " + channel->getName()));
		return ;
	}

	size_t paramIndex = 0;
	size_t paramModeCount = 0;
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

		if (c != 'o' && c != 'i' && c != 't' && c != 'k' && c != 'l')
		{
			client.sendReply(makeReply(Numeric::ERR_UNKNOWNMODE, targetName, std::string(1, c) + " :is unknown mode char to me for " + channel->getName()));
			continue ;
		}

		// 이 모드가 파라미터를 받는지 결정 (-k는 irssi가 키를 보내므로 있으면 소비)
		bool takesParam = (c == 'o') || (c == 'k') || (c == 'l' && enable);
		bool requiresParam = (c == 'o') || (enable && (c == 'k' || c == 'l'));
		std::string param;
		bool hasParam = false;

		if (takesParam)
		{
			if (paramIndex < modeParams.size())
			{
				param = modeParams[paramIndex++];
				hasParam = true;
			}
			else if (requiresParam)
			{
				client.sendReply(makeReply(Numeric::ERR_NEEDMOREPARAMS, targetName, "MODE :Not enough parameters"));
				continue ;			
			}
		}

		// 파라미터를 소비한 뒤에 3개 제한 적용
		if (c == 'o' || c == 'k' || c == 'l')
		{
			if (paramModeCount >= 3)
				continue ;
			paramModeCount++;
		}

		// 'o'는 server에 접근해야 하기 때문에 channel 클래스 안이 아닌 여기서 처리
		if (c == 'o')
		{
			Client *target = server.findClientByNickname(param);
			if (target)
				channel->handleModeOperator(enable, *target, client);
			else
				// RFC의 MODE 목록에 401이 없어 441을 사용
				client.sendReply(makeReply(Numeric::ERR_USERNOTINCHANNEL, targetName, param + " " + channel->getName() + " :They aren't on that channel"));
			continue ;
		}

		// 기능적으로 유의미한 부호만 저장
		char sign = enable ? '+' : '-';
		if (sign != lastAppliedSign)
		{
			filteredModes += sign;
			lastAppliedSign = sign;
		}
		filteredModes += c;
		if (enable && hasParam && (c == 'k' || c == 'l'))
			filteredParams.push_back(param); // -k의 키는 버림
	}
	
	if (!filteredModes.empty())
		channel->handleMode(client, filteredModes, filteredParams);
}