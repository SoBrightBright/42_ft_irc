#include "User.hpp"

#include "../../network/Server.hpp"
#include "../../Client.hpp"
#include "../../parse/Message.hpp"

#include "../IrcHelpers.hpp"

User::User() {}
User::~User() {}
User::User(const User &obj) { (void)obj; }
User	&User::operator=(const User &obj) { (void)obj; return *this; }

bool	User::needLogin() const { return false; }

// USER <username> <mode> <unused> :<realname>: 유저네임/실명 등록. 이미 등록된 클라이언트 재시도 차단, username과 realname 저장
void	User::execute(Server &server, Client &client, const Message &msg) {
	(void)server;

	const std::vector<std::string> &params = msg.getParameter();
	std::string target = "*";
	if (client.isRegistered())
		target = client.getNickname();
	
	if (params.size() < 3 || !msg.hasTrailing()) {
		client.sendReply(IrcReply::formatReplyWithParameter(IrcNumeric::ERR_NEEDMOREPARAMS,
													target, "USER", "Not enough parameters"));
		return ;
	}

	if (client.isRegistered()) {
		client.sendReply(IrcReply::formatReply(IrcNumeric::ERR_ALREADYREGISTERED,
											   target, "You may not register"));
		return ;
	}

	client.setUsername(params[0]);
	client.setRealname(msg.getTrailing());
}
