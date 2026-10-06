#include "Nick.hpp"

#include "../../network/Server.hpp"
#include "../../Client.hpp"
#include "../../parse/Message.hpp"

#include "../IrcHelpers.hpp"

Nick::Nick() {}
Nick::~Nick() {}
Nick::Nick(const Nick &obj) { (void)obj; }
Nick	&Nick::operator=(const Nick &obj) { (void)obj; return *this; }

bool	Nick::needLogin() const { return false; }

static bool	isSpecial(char c) {
	if (c == '[' || c == ']' || c == '\\' || c == '`' ||
		c == '_' || c == '^' || c == '{' || c == '}' || c == '|')
		return true;
	return false;
}

// RFC1459: 첫글자는 문자나 허용된 특수문자 + 8자리 알파벳/숫자/허용된특수문자/-
static bool	isValidNickname(const std::string &newer) {
	if (newer.empty() || newer.size() > 9)
		return false;

	char first = newer[0];
	if (!std::isalpha(static_cast<unsigned char>(first)) && !isSpecial(first))
		return false;

	for (size_t	idx = 1; idx < newer.size(); ++idx) {
		char c = newer[idx];
		if (!std::isalnum(static_cast<unsigned char>(c)) && !isSpecial(c) && c != '-')
			return false;
	}
	return true;
}

// 10/02: 호스트를 IP 따라서.
static std::string nickNotice(const std::string &old, const std::string &newer,
							const std::string &user, const std::string &host) {
	return (":" + old + "!" + user + "@" + host + " NICK :" + newer);
}

// execute 공통: parameter overcontain에 대해서 - 접근조차 없이 무시한다...
// NICK <nickname> : 닉네임 설정/변경. 서버 내 중복 닉네임 체크 및 유효성 검사 후 적용
void	Nick::execute(Server &server, Client &client, const Message &msg) {
	const std::vector<std::string> &params = msg.getParameter();

	if (params.empty() || params[0].empty()) {
		client.sendReply(IrcReply::formatReply(IrcNumeric::ERR_NONICKNAMEGIVEN,
									IrcReply::targetname(client), "No nickname given"));
		return;
	}
	
	bool				wasRegistered = client.isRegistered();
	const std::string	oldNickname = client.getNickname();
	const std::string	&newNickname = params[0];
		
	if (!isValidNickname(newNickname)) {
		client.sendReply(IrcReply::formatReplyWithParameter(IrcNumeric::ERR_ERRONEUSNICKNAME,
									IrcReply::targetname(client), newNickname, "Erroneous nickname"));
		return;
	}

	if (server.isNicknameTaken(newNickname, &client)) {
		client.sendReply(IrcReply::formatReplyWithParameter(IrcNumeric::ERR_NICKNAMEINUSE,
								 	IrcReply::targetname(client), newNickname, "Nickname is already in use"));
		return;
	}

	client.setNickname(newNickname);
	
	if (wasRegistered) {
		if (oldNickname == newNickname)
			return;
	
		std::string	notice = nickNotice(oldNickname, newNickname, client.getUsername(), client.getIp());
		client.broadcastNickChange(notice);
		return;
	}
	
	client.tryCompleteRegistration();
	if (client.isMissingPassword()) {
		client.sendReply(IrcReply::formatReply(IrcNumeric::ERR_PASSWDMISMATCH,
											   "*", "Password incorrect"));
		server.disconnectClient(client, "Bad Password");
	}	
}
