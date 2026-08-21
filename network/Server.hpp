#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <map>
#include <poll.h>
#include "../Client.hpp"
//#include "Channel.hpp"
//#include "Parse.hpp"

class Server
{
    private:
        int                                 _port;
        std::string                         _password;
        int                                 _server_fd;

        std::vector<struct pollfd>          _poll_fds;
        
        std::map<int, Client*>              _clients;

    public:
        Server(int port, const std::string& password);
        ~Server();

        void    init(); 
        void    run();

    private:
        void    acceptNewClient();
        void    receiveData(int client_fd);
        void    sendData(int client_fd);
        void    disconnectClient(int client_fd);
        void    updatePoll(int client_fd);
};

#endif