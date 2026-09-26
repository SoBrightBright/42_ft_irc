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
    if (client.isRegistered() == false)
    {
        for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
        {
            if (it->second == &client)
            {
                client.sendReply(reason);
                delete it->second;
                _clients.erase(it);
                break;
            }
        }
        markForDisconnection(client.getFd(), reason);
        return;
    }
    removeClientFromAllChannels(client, reason);
    markForDisconnection(client.getFd(), reason);
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

void Server::markForDisconnection(int client_fd, const std::string& reason)
{
    if (_to_disconnected.find(client_fd) == _to_disconnected.end())
        _to_disconnected[client_fd] = reason;
}

void Server::cleanupDisconnected()
{
    std::map<int, std::string> targets;

    targets.swap(_to_disconnected);
    for (std::map<int, std::string>::iterator it = targets.begin(); it != targets.end(); ++it)
        disconnect(it->first, it->second);
}


void Server::removeChannelIfEmpty(const std::string& channelName)
{
    (void)channelName;
}

bool Server::isMarked(int client_fd) const
{
    return _to_disconnected.find(client_fd) != _to_disconnected.end();
}