#include "Server.hpp"

Channel* Server::findChannel(const std::string &name)
{
    std::map<std::string, Channel*>::iterator it = _channels.find(name);
    if (it != _channels.end())
        return it->second;
    return NULL;
}

Channel* Server::findOrCreateChannel(const std::string &name)
{
    Channel* channel = findChannel(name);
    if (channel)
        return channel;

    Channel* new_channel = new Channel(name);
    _channels[name] = new_channel;
    return new_channel;
}

Channel* Server::deleteChannel(const std::string &name)
{
    std::map<std::string, Channel*>::iterator it = _channels.find(name);
    if (it != _channels.end())
    {
        Channel* channel = it->second;
        _channels.erase(it);
        delete channel;
        return channel;
    }
    return NULL;
}