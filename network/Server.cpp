#include "Server.hpp"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>

Server::Server(int port, const std::string& password) 
    : _port(port), _password(password), _server_fd(-1) {}

Server::~Server()
{
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        delete it->second;
    }
    if (_server_fd >= 0) {
        close(_server_fd);
    }
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

    struct pollfd server_pollfd;
    server_pollfd.fd = _server_fd;
    server_pollfd.events = POLLIN;
    server_pollfd.revents = 0;
	_poll_fds.push_back(server_pollfd);
}

void Server::run()
{
    std::cout << "IRC Server started on port " << _port << std::endl;

	while (true)
	{
        int poll_count = poll(&_poll_fds[0], _poll_fds.size(), -1);
        if (poll_count < 0) throw std::runtime_error("Poll error");

        for (size_t i = 0; i < _poll_fds.size(); ++i)
        {
            if (_poll_fds[i].revents == 0) continue;

            int current_fd = _poll_fds[i].fd;

            if (current_fd == _server_fd)
            {
                if (_poll_fds[i].revents & POLLIN)
                    acceptNewClient();
                continue;
            }

            if (_poll_fds[i].revents & (POLLERR | POLLHUP)) {
                disconnectClient(current_fd);
                continue;
            }
            if (_poll_fds[i].revents & POLLIN) {
                receiveData(current_fd); 
            }
            if (_poll_fds[i].revents & POLLOUT) {
                sendData(current_fd);
        	}
        }
    }
}

void Server::acceptNewClient()
{
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(_server_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd < 0)
    {
        std::cerr << "Accept failed" << std::endl;
        return;
    }

    if (fcntl(client_fd, F_SETFL, O_NONBLOCK) < 0)
    {
        std::cerr << "fcntl failed for new client" << std::endl;
        close(client_fd);
        return;
    }

    Client* new_client = new Client(client_fd);
    _clients[client_fd] = new_client;

    struct pollfd client_pollfd;
    client_pollfd.fd = client_fd;
    client_pollfd.events = POLLIN; 
    client_pollfd.revents = 0;
    _poll_fds.push_back(client_pollfd);

    std::cout << "New client connected: FD " << client_fd << std::endl;
}

void Server::receiveData(int client_fd)
{
    char buffer[1024];
    ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received < 0)
    {
        if (errno == EWOULDBLOCK || errno == EAGAIN)
            return;
        disconnectClient(client_fd);
        return;
    }
    if (bytes_received == 0) {
        disconnectClient(client_fd);
        return;
    }

    buffer[bytes_received] = '\0';
    _clients[client_fd]->getReadBuffer().append(buffer, bytes_received);
    std::cout << "reciveData 확인" << std::endl;

    while (_clients[client_fd]->hasCompleteLine())
    {
        std::string line = _clients[client_fd]->popLine();
        // 파싱 호출
        // 채널,커맨드 처리 호출

        //이 아래 세 줄은 ECHO 서버 테스트 전용, 지우고 하셔야 irssi 작동.
        std::cout << "solee said: " << '"' << line << '"' << std::endl;
        _clients[client_fd]->getWriteBuffer() += line + "\r\n";
        updatePoll(client_fd);
    }
}

void Server::sendData(int client_fd) {
    Client* client = _clients[client_fd];
    if (!client) return;

    std::string& write_buffer = client-> getWriteBuffer();
    if (write_buffer.empty()) return;

    ssize_t bytes_sent = send(client_fd, write_buffer.c_str(), write_buffer.size(), 0);
    if (bytes_sent < 0)
    {
        std::cerr << "Send error on FD " << client_fd << std::endl;
        return;
    }

    write_buffer.erase(0, bytes_sent);
    updatePoll(client_fd);
}

void Server::disconnectClient(int client_fd)
{
   close(client_fd);

   for(std::vector<pollfd>::iterator it = _poll_fds.begin(); it != _poll_fds.end(); ++it)
   {
       if (it->fd == client_fd)
       {
           _poll_fds.erase(it);
           break;
       }
   }

   // 채널 함수 호출 (빠지라고 전달하는 부분)

    std::map<int, Client*>::iterator it = _clients.find(client_fd);
    if (it != _clients.end())
	{
		delete _clients[client_fd];
		_clients.erase(client_fd);
	}

   std::cout << "Client disconnected: FD" << client_fd << std::endl;
}

void Server::updatePoll(int client_fd) {
    Client* client = _clients[client_fd];
    if (!client) return;
    for (size_t i = 0; i < _poll_fds.size(); ++i) {
        if (_poll_fds[i].fd == client_fd) {
            _poll_fds[i].events = POLLIN;
            if (!client->getWriteBuffer().empty()) {
                _poll_fds[i].events |= POLLOUT;
            }
            break;
        }
    }
}