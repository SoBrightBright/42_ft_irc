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
		client.getWriteBuffer() += makeReply(Numeric::ERR_NEEDMOREPARAMS, target, "JOIN :Not enough parameters");
		return ;
	}

	if (params[0] == "0")
	{
		// TODO: 클라이언트가 속한 모든 채널에서 PART 처리
		// 근데 이건 Client가 "내가 어느 채널들에 있는지" 목록을 가지고 있어야 가능함
		return ;
	}

	// 채널 이름 형식 검증
	if (params[0].empty() || (params[0][0] != '#' && params[0][0] != '&'))
	{
		client.getWriteBuffer() += makeReply(Numeric::ERR_NOSUCHCHANNEL, target, params[0] + " :No such channel");
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

// TODO:: notice about all commands their server receives which affect the channel
// MODE, KICK, PART, QUIT and of course PRIVMSG/NOTICE