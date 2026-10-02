#include "Quit.hpp"

#include "../../network/Server.hpp"
#include "../../Client.hpp"
#include "../../parse/Message.hpp"

#include "../IrcHelpers.hpp"

Quit::Quit() {}
Quit::~Quit() {}
Quit::Quit(const Quit &obj) { (void)obj; }
Quit	&Quit::operator=(const Quit &obj) { (void)obj; return *this; }

bool	Quit::needLogin() const { return false; }

static std::string quitNotice(const std::string &nick, const std::string &user,
							const std::string &host, const std::string &reason) {
	return (":" + nick + "!" + user + "@" + host + " QUIT :" + reason);
}

// QUIT [:reason] : 연결 종료. 종료 사유 추출 및 실제 연결 종료+broadcast
void	Quit::execute(Server &server, Client &client, const Message &msg) {
	std::string reason = "Client Quit";
	if (msg.hasTrailing())
		reason = msg.getTrailing();
	
	if (client.isRegistered()) {
		std::string notice = quitNotice(client.getNickname(), client.getUsername(), client.getIp(), reason);
		client.broadcastQuit(notice);
	}
	server.disconnectClient(client, reason);
}
