#include "Server.hpp"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <stdexcept>

static struct pollfd makePollFd(int fd, short events)
{
    struct pollfd poll_fd;
    poll_fd.fd = fd;
    poll_fd.events = events;
    poll_fd.revents = 0;
    return poll_fd;
}

Server::Server(int port, const std::string& password)
    : _port(port), _server_fd(-1), _password(password) {}

Server::~Server()
{
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
        delete it->second;
    for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
        delete it->second;
    if (_server_fd >= 0)
        close(_server_fd);
}

void Server::init()
{
    _server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_fd < 0)
        throw std::runtime_error("Socket creation failed");

    if (fcntl(_server_fd, F_SETFL, O_NONBLOCK) < 0)
        throw std::runtime_error("fcntl failed");

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(_port);

    if (bind(_server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
        throw std::runtime_error("Bind failed");

    if (listen(_server_fd, SOMAXCONN) < 0)
        throw std::runtime_error("Listen failed");

    _poll_fds.push_back(makePollFd(_server_fd, POLLIN));
}

void Server::run()
{
    std::cout << "IRC Server started on port " << _port << std::endl;

    while (true)
    {
        for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
        {
            if (it->second->getIdleTime() > TIMEOUT_SECONDS)
                disconnectClient(*(it->second), "Ping timeout");
        }

        int poll_count = poll(&_poll_fds[0], _poll_fds.size(), 1000);
        if (poll_count < 0)
            throw std::runtime_error("Poll error");

        for (size_t i = 0; i < _poll_fds.size(); ++i)
        {
            if (_poll_fds[i].revents == 0)
                continue;

            int current_fd = _poll_fds[i].fd;

            if (current_fd == _server_fd)
            {
                if (_poll_fds[i].revents & POLLIN)
                    acceptNewClient();
                continue;
            }

            if (_poll_fds[i].revents & (POLLERR | POLLHUP))
            {
                markForDisconnection(current_fd);
                continue;
            }
            if (_poll_fds[i].revents & POLLIN)
                receiveData(current_fd);
            if (_poll_fds[i].revents & POLLOUT)
                sendData(current_fd);
        }
        cleanupDisconnected();
    }
}

void Server::updatePoll()
{
    for (size_t i = 0; i < _poll_fds.size(); ++i)
    {
        if (_poll_fds[i].fd == _server_fd)
            continue;

        std::map<int, Client*>::iterator it = _clients.find(_poll_fds[i].fd);
        if (it == _clients.end())
            continue;

        _poll_fds[i].events = POLLIN;
        if (!it->second->getWriteBuffer().empty())
            _poll_fds[i].events |= POLLOUT;
    }
}
