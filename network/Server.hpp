#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <map>
#include <poll.h>
#include <arpa/inet.h>
#include <csignal>
#include "../Client.hpp"
#include "../channel/Channel.hpp"
#include "../parse/Message.hpp"
#include "../parse/Parser.hpp"

// hpp에 선언 (상수값은 임의. 합의 하에 90~300에서 정하면 됨)
const long TIMEOUT_SECONDS = 300;

extern volatile std::sig_atomic_t g_stop;


class Server
{
    private:
        int	                            _port;
        int                             _server_fd;
        std::string                     _password;

        Parser                         _parser;

        std::vector<struct pollfd>      _poll_fds;

        std::map<int, Client*>          _clients;
        std::map<std::string, Channel*> _channels; // string은 채널이름, Channel*은 채널의 포인터

        std::map<int, std::string>     _to_disconnected; // client_fd -> reason

        void    acceptNewClient();
        void    receiveData(int client_fd);
        void    sendData(int client_fd);
        void    refreshPollEvents();

        void    cleanupDisconnected();
        void    disconnect(int client_fd, const std::string& reason);
        void    removeClientFromAllChannels(Client&, const std::string& reason);

        bool    isMarked(int client_fd) const;

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
		Channel* deleteChannel(const std::string &name); // name으로 찾아서 삭제, 없으면 NULL 반환

        Client* findClientByNickname(const std::string &nickname); // 지금까지 몇 번 말씀드렸던 것!! nickname으로 Client * 찾아서 반환. 없으면 NULL 반환

        void    removeChannelIfEmpty(const std::string &channelName);
        void    markForDisconnection(int client_fd, const std::string& reason);        
    };

#endif