#include "Quit.hpp"

#include "../../network/Server.hpp"
#include "../../Client.hpp"
#include "../../parse/Message.hpp"

#include "../IrcHelpers.hpp"

#include <vector>

Quit::Quit() {}
Quit::~Quit() {}
Quit::Quit(const Quit &obj) { (void)obj; }
Quit	&Quit::operator=(const Quit &obj) { (void)obj; return *this; }

bool	Quit::needLogin() const { return false; }

// QUIT [:reason] : 연결 종료. 종료 사유 추출 및 실제 연결 종료+broadcast
void	Quit::execute(Server &server, Client &client, const Message &msg) {
	const std::vector<std::string> &params = msg.getParameter();

	std::string reason = "Client Quit";
	if (!params.empty() && !params[0].empty())
		reason = params[0];
	
	server.disconnectClient(client, reason);
}
