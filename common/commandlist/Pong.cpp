#include "Pong.hpp"

#include "../../network/Server.hpp"
#include "../../Client.hpp"
#include "../../parse/Message.hpp"

#include "../IrcHelpers.hpp"

Pong::Pong() {}
Pong::~Pong() {}
Pong::Pong(const Pong &obj) { (void)obj; }
Pong	&Pong::operator=(const Pong &obj) { (void)obj; return *this; }

bool	Pong::needLogin() const { return false; }

// PONG <token>: client->server 연결상태 확인 응답. 활동시간 갱신
void	Pong::execute(Server &server, Client &client, const Message &msg) {
	(void)server;
	(void)msg;
	client.updateLastActivity();
}
