#include "./network/Server.hpp"
#include <iostream>
#include <cstdlib>

int main(int argc, char **argv)
{
    if (argc != 3) {
        std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
        return 1;
    }

    int port = std::atoi(argv[1]);
    std::string password = argv[2];

    if (port <= 0 || port > 65535) {
        std::cerr << "Error: Invalid port number" << std::endl;
        return 1;
    }

    try {
        Server irc_server(port, password); // 서버 객체 생성
        irc_server.init(); // 소켓 셋업, 논블로킹
        irc_server.run();  // poll() 무한 루프
    } catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
