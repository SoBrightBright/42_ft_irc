#include "Server.hpp"
#include <iostream>
#include <cctype>

std::string Server::getName()
{
    return "ft_irc";
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
    markForDisconnection(client.getFd(), reason);
}

Client* Server::findClientByNickname(const std::string& nickname)
{
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (it->second->getNickname() == nickname)
            return it->second;
    }
    return NULL;
}
