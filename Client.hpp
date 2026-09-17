#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>
# include <map>
//# include "channel/Channel.hpp"

class Client
{
    private:
        int         _fd;                 // 클라이언트의 소켓 파일 디스크립터
        std::string _ip;                 // 클라이언트의 IP 주소
        std::string _nickname;           // NICK 명령어로 설정할 닉네임
        std::string _username;           // USER 명령어로 설정할 유저네임
        std::string _realname;           // USER 명령어로 설정할 실명

        bool        _is_authenticated;   // PASS 명령어를 통한 인증 완료 여부
        bool        _is_registered;      // NICK과 USER 설정까지 모두 마쳤는지 여부
        bool        _is_password_verified; // 비밀번호 검증 여부

        //std::map<std::string, Channel*> _joinedChannels; // 클라이언트가 참여한 채널 목록

        std::string _read_buffer;        // 수신된 조각난 데이터를 모아두는 버퍼 (solee -> jimkim)
        std::string _write_buffer;       // 전송을 대기 중인 데이터를 모아두는 버퍼 (soolee -> solee)
        
        // 들어가있는 채널 목록
    public:
        Client(int fd);
        Client(const Client& other);
        Client &operator=(const Client& other);
        ~Client();

        int             getFd() const;
        std::string     getIp() const;
        std::string     getNickname() const;
        std::string     getUsername() const;
        std::string&    getReadBuffer();
        std::string&    getWriteBuffer();
        std::string     getRealname() const;

        void        setIp(const std::string& ip);
        void        setNickname(const std::string& nickname);
        void        setUsername(const std::string& username);
        void        setRealname(const std::string& realname);

        void        setAuthenticated(bool status);
        void        setRegistered(bool status);
        void        setPasswordVerified(bool status);

        void        sendReply(const std::string& message);

        bool        hasCompleteLine() const;
        bool        hasPasswordVerified() const;

        bool        isAuthenticated() const;
        bool        isRegistered() const;
        std::string popLine();
};

#endif
