#include "Server.hpp"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <stdexcept>

static volatile std::sig_atomic_t g_running = 1;
static void handleSignal(int sig)
{
    (void)sig;
    g_running = 0;
}

Server::Server(int port, const std::string& password)
    : _port(port), _server_fd(-1), _password(password) {}

Server::~Server()
{
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        close(it->first);
        delete it->second;
    }
    _clients.clear();
    for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
        delete it->second;
    _channels.clear();
    _poll_fds.clear();
    if (_server_fd >= 0)
        close(_server_fd);
}

void Server::init()
{
    _server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_fd < 0)
        throw std::runtime_error("Socket creation failed");

    int opt = 1; // 재시작 주소 에러 방지용
    if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        throw std::runtime_error("setsockept failed");

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

    struct pollfd server_pollfd;
    server_pollfd.fd = _server_fd;
    server_pollfd.events = POLLIN;
    server_pollfd.revents = 0;
    _poll_fds.push_back(server_pollfd);
}

void Server::run()
{
    signal(SIGINT, handleSignal);
    signal(SIGQUIT, handleSignal);
    signal(SIGPIPE, SIG_IGN);

    std::cout << "IRC Server started on port " << _port << std::endl;

    while (!g_stop)
    {
        refreshPollEvents();
        int ready = poll(&_poll_fds[0], _poll_fds.size(), -1);
        if (ready < 0)
        {
            if (!g_running)
                break;
            continue;
        }

        size_t count = _poll_fds.size();
        for (size_t i = 0; i < count; ++i)
        {
            struct pollfd p = _poll_fds[i];
            if (p.revents == 0)
                continue;
            if (p.fd == _server_fd)
            {
                if (p.revents & POLLIN)
                    acceptNewClient();
                continue;
            }
            if (p.revents & (POLLERR | POLLNVAL))
            {
                markForDisconnection(p.fd, "Connection error");
                continue;
            }
            if (p.revents & (POLLIN | POLLHUP))
                receiveData(p.fd);
            if ((p.revents & POLLOUT) && !isMarked(p.fd))
                sendData(p.fd);
        }
        cleanupDisconnected();
    }
}

void Server::refreshPollEvents()
{
    for (size_t i = 0; i < _poll_fds.size(); ++i)
    {
        if (_poll_fds[i].fd == _server_fd)
            continue;

        _poll_fds[i].events = POLLIN;
        std::map<int, Client*>::iterator it = _clients.find(_poll_fds[i].fd);

        if(it != _clients.end() && !it->second->getWriteBuffer().empty())
            _poll_fds[i].events |= POLLOUT;
    }
}
