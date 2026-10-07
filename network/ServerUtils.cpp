#include "Server.hpp"
#include "../common/IrcHelpers.hpp"

std::string Server::getName()
{
    return "ircserv";
}

bool Server::isNicknameTaken(const std::string& nickname, const Client *self)
{
    std::string nick = IrcText::toLower(nickname);

    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (it->second == self)
            continue;
        if (IrcText::toLower(it->second->getNickname()) == nick)
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
            IrcText::toLower(it->second->getNickname()) == IrcText::toLower(nickname))
            return it->second;
    }
    return NULL;
}
