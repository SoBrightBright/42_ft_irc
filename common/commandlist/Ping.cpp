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

// PING <token>: client->server 연결상태 확인. 응답 전송(POng <token> 되돌려주기)
void	Ping::execute(Server &server, Client &client, const Message &msg) {
	(void)server;

	const std::vector<std::string> &params = msg.getParameter();
	std::string	token;

	if (!params.empty())
		token = params[0];
	else if (msg.hasTrailing())
		token = msg.getTrailing();
	else {
		std::string target = "*";
		if (client.isRegistered()) { 
			target = client.getNickname();
			client.sendReply(IrcReply::formatReply(IrcNumeric::ERR_NOORIGIN,
											 target, "No origin specified"));
			return;
		}
	}

	client.sendReply(IrcReply::formatCommand("PONG", token));
}
