#include "Server.hpp"

void Server::cleanupDisconnected()
{
    std::map<int, std::string> targets;

    targets.swap(_to_disconnected);
    for (std::map<int, std::string>::iterator it = targets.begin(); it != targets.end(); ++it)
        disconnect(it->first, it->second);
}

void Server::disconnect(int client_fd, const std::string& reason)
{
    std::map<int, Client*>::iterator it = _clients.find(client_fd);
    if (it != _clients.end())
    {
        removeClientFromAllChannels(*it->second, reason);
        delete it->second;
        _clients.erase(it);
    }
    
    for (std::vector<pollfd>::iterator poll_it = _poll_fds.begin(); poll_it != _poll_fds.end(); ++poll_it)
    {
        if (poll_it->fd == client_fd)
        {
            _poll_fds.erase(poll_it);
            break;
        }
    }
    _closing.erase(client_fd);
    close(client_fd);

    std::cout << "Client disconnected: FD " << client_fd << " (" << reason << ")" << std::endl;
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

bool Server::isMarked(int client_fd) const
{
    return _to_disconnected.find(client_fd) != _to_disconnected.end();
}

void Server::markForDisconnection(int client_fd, const std::string& reason)
{
    if (_to_disconnected.find(client_fd) == _to_disconnected.end())
        _to_disconnected[client_fd] = reason;
}

void Server::disconnectAfterFlush(Client &c, const std::string &reason)
{
    c.sendReply("ERROR :Closing Link: " + c.getIp() + " (" + reason + ")");
    _closing[c.getFd()] = reason;
}