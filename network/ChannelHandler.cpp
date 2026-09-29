#include "Server.hpp"

std::string tolower_name(const std::string &name)
{
    std::string lower_name = name;
    for (size_t i = 0; i < lower_name.length(); ++i)
        lower_name[i] = std::tolower(lower_name[i]);
    return (lower_name);
}

Channel* Server::findChannel(const std::string &name)
{
    std::map<std::string, Channel*>::iterator it = _channels.find(tolower_name(name));
    if (it != _channels.end())
        return it->second;
    return NULL;
}

Channel* Server::findOrCreateChannel(const std::string &name)
{
    Channel* channel = findChannel(tolower_name(name));
    if (channel)
        return channel;

    Channel* new_channel = new Channel(name);
    _channels[name] = new_channel;
    return new_channel;
}

void Server::deleteChannel(const std::string &name)
{
    std::map<std::string, Channel*>::iterator it = _channels.find(tolower_name(name));
    if (it != _channels.end())
    {
        Channel* channel = it->second;
        _channels.erase(it);
        delete channel;
    }
}