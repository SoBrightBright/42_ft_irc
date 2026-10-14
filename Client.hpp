#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>
# include <map>
# include <ctime>
# include <vector>
# include "common/IrcHelpers.hpp"

class Channel;

class Client
{
    private:
        int         _fd;
        std::string _ip;
        std::string _nickname;
        std::string _username;
        std::string _realname;

        bool        _is_registered;
        bool        _is_password_verified;

        std::map<std::string, Channel*> _joinedChannels; // 클라이언트가 참여한 채널 목록
        std::vector<Channel*> _invitedChannels; // 클라이언트가 초대된 채널 목록

        std::string _read_buffer;
        std::string _write_buffer;

        std::time_t _connectedAt;
        std::time_t _lastActivity; // 마지막 활동 시간 (초 단위)
        bool        _pingSent;
        std::time_t _closingSince;

    public:
        Client(int fd);
        ~Client();

        int             getFd() const;
        std::string     getIp() const;
        std::string     getNickname() const;
        std::string     getUsername() const;
        std::string&    getReadBuffer();
        std::string&    getWriteBuffer();
        std::string     getRealname() const;
        const std::map<std::string, Channel*>& getJoinedChannels();
        const std::vector<Channel*> &getInvitedChannels() const;

        void        setIp(const std::string& ip);
        void        setNickname(const std::string& nickname);
        void        setUsername(const std::string& username);
        void        setRealname(const std::string& realname);
        void        setRegistered(bool status);
        void        setPasswordVerified(bool status);

        void        addJoinedChannels(const std::string& name, Channel* channel);
        void        removeJoinedChannels(const std::string& name);

        void        addInvitedChannel(Channel *ch);
        void        removeInvitedChannel(Channel *ch);

        bool        hasCompleteLine() const;
        void        sendReply(const std::string& message);
        std::string popLine();

        bool        hasPasswordVerified() const;

        bool        isRegistered() const;

        void        updateLastActivity();
        long        getIdleTime() const;
        long        getConnectTime() const;

        bool        isPingSent() const;
        void        setPingSent(bool status);
        void        markClosing();
        long        getClosingTime() const;

        void        tryCompleteRegistration();
        bool        isMissingPassword() const;

        void        broadcastNickChange(const std::string &notice);
        void        broadcastQuit(const std::string &notice);
};


#endif
