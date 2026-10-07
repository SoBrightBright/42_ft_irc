#include "User.hpp"

#include "../../network/Server.hpp"
#include "../../Client.hpp"
#include "../../parse/Message.hpp"

#include "../IrcHelpers.hpp"

#include <iostream>

User::User() {}
User::~User() {}
User::User(const User &obj) { (void)obj; }
User	&User::operator=(const User &obj) { (void)obj; return *this; }

bool	User::needLogin() const { return false; }

// `USER <username> <mode> <unused> :<realname>`: 유저네임/실명 등록. 이미 등록된 클라이언트 재시도 차단, username과 realname 저장
void	User::execute(Server &server, Client &client, const Message &msg) {
	(void)server;

	const std::vector<std::string> &params = msg.getParameter();
	
	if (params.size() < 4 || params[0].empty()) {
		client.sendReply(IrcReply::formatReplyWithParameter(IrcNumeric::ERR_NEEDMOREPARAMS,
										IrcReply::targetname(client), "USER", "Not enough parameters"));
		return ;
	}

	if (client.isRegistered()) {
		client.sendReply(IrcReply::formatReply(IrcNumeric::ERR_ALREADYREGISTERED,
										IrcReply::targetname(client), "You may not reregister"));
		return ;
	}

	// RFC2812 2.3.1: user에는 '@'가 들어갈 수 없다.
	// RFC에는 '잘못된 username' 전용 응답이 없으므로 461 error로 거부.
	if (params[0].find('@') != std::string::npos) {
		client.sendReply(IrcReply::formatReplyWithParameter(IrcNumeric::ERR_NEEDMOREPARAMS,
										IrcReply::targetname(client), "USER", "Not enough parameters"));
		return;
	}

	client.setUsername(params[0]);
	client.setRealname(params[3]);
	client.tryCompleteRegistration();

	if (client.isMissingPassword()) {
		client.sendReply(IrcReply::formatReply(IrcNumeric::ERR_PASSWDMISMATCH,
											   "*", "Password incorrect"));
		server.disconnectClient(client, "Bad Password");
	}	
}
