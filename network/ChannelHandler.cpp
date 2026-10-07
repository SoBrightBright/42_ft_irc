#include "Server.hpp"
#include "../common/IrcHelpers.hpp"

Channel* Server::findChannel(const std::string &name)
{
    std::map<std::string, Channel*>::iterator it = _channels.find(IrcText::toLower(name));
    if (it != _channels.end())
        return it->second;
    return NULL;
}

Channel* Server::findOrCreateChannel(const std::string &name)
{
    std::string key = IrcText::toLower(name);
    Channel* channel = findChannel(key);
    if (channel)
        return channel;

    Channel* new_channel = new Channel(name);
    _channels[key] = new_channel;
    return new_channel;
}

void Server::clearInvitedUsers(Channel *channel)
{
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
        it->second->removeInvitedChannel(channel);
}

void Server::deleteChannel(const std::string &name)
{
    std::map<std::string, Channel*>::iterator it = _channels.find(IrcText::toLower(name));
    if (it != _channels.end())
    {
        Channel* channel = it->second;
        _channels.erase(it);
        clearInvitedUsers(channel);
        delete channel;
    }
}