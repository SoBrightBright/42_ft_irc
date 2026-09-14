#include "Server.hpp"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>

Server::Server(int port, const std::string& password) 
    : _port(port), _server_fd(-1), _password(password) {}

Server::~Server()
{
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        delete it->second;
    }
    for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
    {
        delete it->second;
    }
    if (_server_fd >= 0) {
        close(_server_fd);
    }
}

void Server::init()
{
    _server_fd = socket(AF_INET, SOCK_STREAM, 0); //소켓 생성
    if (_server_fd < 0)
        throw std::runtime_error("Socket creation failed");

    if (fcntl(_server_fd, F_SETFL, O_NONBLOCK) < 0) // 논블로킹 모드 설정
        throw std::runtime_error("fcntl failed");

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY; // 모든 인터페이스 연결 수락
    server_addr.sin_port = htons(_port); // 포트 설정

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
        int poll_count = poll(&_poll_fds[0], _poll_fds.size(), -1); // poll() 호출, -1은 무한 대기
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

void Server::markForDisconnection(int client_fd) // 클라이언트 정리하기 위해 _to_disconnected 벡터에 추가
{
    for (size_t i = 0; i < _to_disconnected.size(); ++i)
        if (_to_disconnected[i] == client_fd) return;
    _to_disconnected.push_back(client_fd);
}

void Server::cleanupDisconnected() // _to_disconnected 벡터에 있는 클라이언트들 정리
{
    for (size_t i = 0; i < _to_disconnected.size(); ++i)
        disconnectClient(_to_disconnected[i]);
    _to_disconnected.clear();
}

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
    int client_fd = client.getFd();
    disconnectClient(client_fd);
    std::cout << "Client disconnected: FD " << client_fd << ", Reason: " << reason << std::endl;
}

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

Client* Server::findClientByNickname(const std::string &nickname)
{
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (it->second->getNickname() == nickname)
            return it->second;
    }
    return NULL;
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

    if (fcntl(client_fd, F_SETFL, O_NONBLOCK) < 0)
    {
        std::cerr << "fcntl failed for new client" << std::endl;
        close(client_fd);
        return;
    }

    Client* new_client = new Client(client_fd);
    _clients[client_fd] = new_client;

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
    new_client->setIp(client_ip);

    struct pollfd client_pollfd;
    client_pollfd.fd = client_fd;
    client_pollfd.events = POLLIN; 
    client_pollfd.revents = 0;
    _poll_fds.push_back(client_pollfd);

    std::cout << "test: " << client_ip << std::endl;
    std::cout << "test: " << ntohs(client_addr.sin_port) << std::endl;
    // 클라이언트 ip 필요 시 

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
        markForDisconnection(client_fd);
        return;
    }
    if (bytes_received == 0)
    {
        markForDisconnection(client_fd);
        return;
    }

    buffer[bytes_received] = '\0';
    _clients[client_fd]->getReadBuffer().append(buffer, bytes_received);
    std::cout << "receiveData 확인" << std::endl;

    while (_clients[client_fd]->hasCompleteLine())
    {
        std::string line = _clients[client_fd]->popLine();
        
        // Message msg; 이 밑으로 파싱하고 연결하는 부분
        // if (!msg.spliter(line))
        // {
        //     std::cerr << "Failed to parse message: " << line << std::endl;
        //     continue;
        // }
        // commandHandler.dispatch(_clients[client_fd], msg);
    }
}

void Server::sendData(int client_fd) {
    Client* client = _clients[client_fd];
    if (!client) return;

    std::string& write_buffer = client-> getWriteBuffer(); // 클라이언트의 write_buffer 가져오기
    if (write_buffer.empty()) return;

    ssize_t bytes_sent = send(client_fd, write_buffer.c_str(), write_buffer.size(), 0);
    if (bytes_sent < 0)
    {
        if (errno == EWOULDBLOCK || errno == EAGAIN) //EWOULDBLOCK과 EAGAIN은 send가 블로킹되지 않고, 즉시 반환되었음을 나타냄
            return;
        markForDisconnection(client_fd);
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

   // channelManager.removeClientFromAllChannels(client_fd); // 채널에서 클라이언트 제거

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
