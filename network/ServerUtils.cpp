#include "Server.hpp"

std::string Server::getName()
{
    return "ircserv";
}

static std::string  toLowerStr(const std::string& str)
{
    std::string rts = str;

    for (size_t idx = 0; idx < rts.size(); ++idx)
        rts[idx] = std::tolower(static_cast<unsigned char>(rts[idx]));
    return rts;
}

bool Server::isNicknameTaken(const std::string& nickname, const Client *self)
{
    std::string nick = toLowerStr(nickname);

    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (it->second == self)
            continue;
        if (toLowerStr(it->second->getNickname()) == nick)
            return true;
    }
    return false;
}

bool Server::checkPassword(const std::string& password) const
{
    return _password == password;
}

void Server::disconnectClient(Client& client, const std::string& reason)
{
    disconnectAfterFlush(client, reason);
}

Client* Server::findClientByNickname(const std::string& nickname)
{
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (it->second->isRegistered() &&
            toLowerStr(it->second->getNickname()) == toLowerStr(nickname))
            return it->second;
    }
    return NULL;
}
