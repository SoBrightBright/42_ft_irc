#include "Pass.hpp"

#include "../../network/Server.hpp"
#include "../../Client.hpp"
#include "../../parse/Message.hpp"

#include "../IrcHelpers.hpp"

Pass::Pass() {}
Pass::~Pass() {}
Pass::Pass(const Pass &obj) { (void)obj; }
Pass	&Pass::operator=(const Pass &obj) { (void)obj; return *this; }

bool	Pass::needLogin() const { return false; }

// 10/02: 메세지 파라미터 변경으로 인한 코드 수정 & 함수 통일
// PASS <password>: 서버 접속 비밀번호 확인. 이미 등록된 클라이언트 재시도 차단, 비밀번호 대조 및 통과 상태 저장
void	Pass::execute(Server &server, Client &client, const Message &msg) {
	const std::vector<std::string> &params = msg.getParameter();

	if (params.empty() || params[0].empty()) {
		client.sendReply(IrcReply::formatReplyWithParameter(IrcNumeric::ERR_NEEDMOREPARAMS,
							IrcReply::targetname(client), "PASS", "Not enough parameters"));
		return;
	}
	
	if (client.isRegistered()) {
			client.sendReply(IrcReply::formatReply(IrcNumeric::ERR_ALREADYREGISTERED,
							IrcReply::targetname(client), "You may not reregister"));
		return;
	}
	
	if (!server.checkPassword(params[0])) {
			client.sendReply(IrcReply::formatReply(IrcNumeric::ERR_PASSWDMISMATCH,
							IrcReply::targetname(client), "You may not reregister"));	
		}

	client.setAuthenticated(true);
}
