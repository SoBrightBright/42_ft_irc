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

// 10/02: 메세지 파라미터 변경으로 인한 코드 수정 & 함수 통일
// USER <username> <mode> <unused> :<realname>: 유저네임/실명 등록. 이미 등록된 클라이언트 재시도 차단, username과 realname 저장
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

	client.setUsername(params[0]);
<<<<<<< Updated upstream
	// client.setRealname(msg.getTrailing());
=======
	client.setRealname(params[3]);
	client.tryCompleteRegistration();
>>>>>>> Stashed changes
}
