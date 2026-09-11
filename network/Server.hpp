#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <map>
#include <poll.h>
#include <arpa/inet.h>
#include "../Client.hpp"
#include "../channel/Channel.hpp"
#include "../parse/Message.hpp"

class Server
{
    private:
        int	                            _port;
        int                             _server_fd;
        std::string                     _password;

        std::vector<struct pollfd>      _poll_fds;

        std::map<int, Client*>          _clients;
        std::map<std::string, Channel*> _channels;

        std::vector<int>                     _to_disconnected;

    public:
        Server(int port, const std::string& password);
        ~Server();

        void    init(); 
        void    run();

        std::string getName();
        bool    isNicknameTaken(const std::string& nickname);
        bool    checkPassword(const std::string& password) const;
        void    disconnectClient(Client&, const std::string& reason);

        Channel* findChannel(const std::string &name); // name으로 Channel*을 찾아서 반환, 없으면 NULL 반환
        Channel* findOrCreateChannel(const std::string &name); // JOIN에서 사용, name으로 찾아보고 없으면 name이란 채널을 새로 만듦
        Client* findClientByNickname(const std::string &nickname); // 지금까지 몇 번 말씀드렸던 것!! nickname으로 Client * 찾아서 반환. 없으면 NULL 반환

    private:
        void    acceptNewClient();
        void    receiveData(int client_fd);
        void    sendData(int client_fd);
        void    disconnectClient(int client_fd);
        void    updatePoll(int client_fd);

        void    markForDisconnection(int client_fd);
        void    cleanupDisconnected();

        void    removeChannelIfEmpty(const std::string &channelName);
    };

#endif