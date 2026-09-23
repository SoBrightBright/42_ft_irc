#include "Server.hpp"
#include <iostream>

std::string Server::getName()
{
    return "ft_irc";
}

bool Server::isNicknameTaken(const std::string& nickname)
{
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (it->second->getNickname() == nickname)
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
    removeClientFromAllChannels(client, reason);
    markForDisconnection(client.getFd());
}

void Server::removeClientFromAllChannels(Client& client, const std::string& reason)
{
    std::vector<std::string> emptied;
    for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
    {
        if (!it->second->isMember(client))
            continue;
        it->second->handlePart(client, reason);
        if (it->second->isEmpty())
            emptied.push_back(it->first);
    }
    for (size_t i = 0; i < emptied.size(); ++i)
        deleteChannel(emptied[i]);
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

void Server::markForDisconnection(int client_fd)
{
    for (size_t i = 0; i < _to_disconnected.size(); ++i)
    {
        if (_to_disconnected[i] == client_fd)
            return;
    }
    _to_disconnected.push_back(client_fd);
}

void Server::cleanupDisconnected()
{
    std::vector<int> pending;
    pending.swap(_to_disconnected);

    for (size_t i = 0; i < pending.size(); ++i)
        disconnectClient(pending[i]);
}

void Server::removeChannelIfEmpty(const std::string& channelName)
{
    (void)channelName;
}
