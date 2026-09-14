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
	if (!std::isalpha(static_cast<unsigned char>(first) && !isSpecial(first)))
		return false;

	for (size_t	idx = 1; idx < newer.size(); ++idx) {
		char c = newer[idx];
		if (!std::isalnum(static_cast<unsigned char>(c) && !isSpecial(c)) && c != '-')
			return false;
	}
	return true;
}

// execute 공통: parameter overcontain에 대해서 - 접근조차 없이 무시한다...
// NICK <nickname> : 닉네임 설정/변경. 서버 내 중복 닉네임 체크 및 유효성 검사 후 적용
void	Nick::execute(Server &server, Client &client, const Message &msg) {
	const std::vector<std::string> &params = msg.getParameter();

	std::string	target = "*";
	if (client.isRegistered())
		target = client.getNickname();

	if (params.empty()) {
		client.sendReply(IrcReply::formatReply(IrcNumeric::ERR_NONICKNAMEGIVEN,
											   target, "No nickname given"));

		const std::string &newNickname = params[0];
		
		if (!isValidNickname(newNickname)) {
			client.sendReply(IrcReply::formatReplyWithParameter(IrcNumeric::ERR_ERRONEUSNICKNAME,
								 					target, newNickname, "Erroneous nickname"));
			return;
		}

		if (server.isNicknameTaken(newNickname)) {
			client.sendReply(IrcReply::formatReplyWithParameter(IrcNumeric::ERR_ERRNICKNAMEINUSE,
								 				target, newNickname, "Nickname is already in use"));
		}

		client.setNickname(newNickname);
	}
}
