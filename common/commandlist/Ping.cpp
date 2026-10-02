#include "Ping.hpp"

#include "../../network/Server.hpp"
#include "../../Client.hpp"
#include "../../parse/Message.hpp"

#include "../IrcHelpers.hpp"

Ping::Ping() {}
Ping::~Ping() {}
Ping::Ping(const Ping &obj) { (void)obj; }
Ping	&Ping::operator=(const Ping &obj) { (void)obj; return *this; }

bool	Ping::needLogin() const { return false; }

// 10/02: 메세지 파라미터 변경으로 인한 코드 수정 & 함수 통일
// PING <token>: client->server 연결상태 확인. 응답 전송(Pong <token> 되돌려주기)
void	Ping::execute(Server &server, Client &client, const Message &msg) {
	(void)server;

	const std::vector<std::string> &params = msg.getParameter();

	// RFC2812 3.7.2, 3.7.3: 등록 여부와 상관없이, 토큰 없으면 409
	if (params.empty() || params[0].empty()) {
		client.sendReply(IrcReply::formatReply(IrcNumeric::ERR_NOORIGIN,
					IrcReply::targetname(client), "No origin specified"));
		return;
	}

	client.sendReply(":ircserv PONG ircserv :" + params[0]);
}
