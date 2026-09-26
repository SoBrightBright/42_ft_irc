#include "Server.hpp"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>

static void setNonBlocking(int fd)
{
    if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
        throw std::runtime_error("fcntl failed");
}

static struct pollfd createPollFd(int fd)
{
    struct pollfd poll_fd;
    poll_fd.fd = fd;
    poll_fd.events = POLLIN;
    poll_fd.revents = 0;
    return poll_fd;
}

void Server::acceptNewClient()
{
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(_server_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd < 0)
    {
        if (errno != EWOULDBLOCK && errno != EAGAIN)
            std::cerr << "Accept failed" << std::endl;
        return;
    }

    try
    {
        setNonBlocking(client_fd);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << " for new client" << std::endl;
        close(client_fd);
        return;
    }

    Client* new_client = new Client(client_fd);
    _clients[client_fd] = new_client;

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
    new_client->setIp(client_ip);

    _poll_fds.push_back(createPollFd(client_fd));

    std::cout << "New client connected: FD " << client_fd << std::endl;
}

void Server::receiveData(int client_fd)
{
    std::map<int, Client*>::iterator it = _clients.find(client_fd);
    if (it == _clients.end())
        return;

    Client* client = it->second;

    char buffer[1024];
    ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
    if (bytes_received < 0)
    {
        if (errno == EWOULDBLOCK || errno == EAGAIN)
            return;
        markForDisconnection(client_fd, "Receive error");
        return;
    }
    if (bytes_received == 0)
    {
        markForDisconnection(client_fd, "Client disconnected");
        return;
    }

    client->getReadBuffer().append(buffer, bytes_received);
    client->updateLastActivity();

    while (client->getReadBuffer().size() > 8192 && !client->hasCompleteLine())
    {
        markForDisconnection(client_fd, "Input buffer overflow");
        return;
    }

    while (!isMarked(client_fd) && client->hasCompleteLine())
    {
        std::string line = client->popLine();
        if (line.empty())
            continue;

        if (!_parser.operate(*this, *client, line))
            std::cerr << "Failed to parse message: " << line << std::endl;
    }
}

void Server::sendData(int client_fd)
{
    std::map<int, Client*>::iterator it = _clients.find(client_fd);
    if (it == _clients.end())
        return;

    std::string& write_buffer = it->second->getWriteBuffer();
    if (write_buffer.empty())
        return;

    ssize_t bytes_sent = send(client_fd, write_buffer.c_str(), write_buffer.size(), 0);
    if (bytes_sent < 0)
    {
        if (errno == EWOULDBLOCK || errno == EAGAIN)
            return;
        markForDisconnection(client_fd, "Send error");
        return;
    }
    write_buffer.erase(0, bytes_sent);
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
    close(client_fd);

    std::cout << "Client disconnected: FD " << client_fd << " (" << reason << ")" << std::endl;
}
