#include "Client.hpp"
#include "channel/Channel.hpp"
#include <iostream>
#include <string>
#include <algorithm>
#include <set>

// Constructors and Destructors
Client::Client(int fd):
	_fd(fd), _nickname(""), _username(""), _realname(""),
	_is_registered(false), _is_password_verified(false), _read_buffer(""), _write_buffer(""),
	_connectedAt(std::time(NULL)), _lastActivity(std::time(NULL)), _pingSent(false), _closingSince(0) {}

Client::~Client() {}

// Getters
int Client::getFd() const { return _fd; }

std::string Client::getIp() const { return _ip; }

std::string Client::getNickname() const { return _nickname; }

std::string Client::getUsername() const { return _username; }

std::string& Client::getReadBuffer() { return _read_buffer; }

std::string& Client::getWriteBuffer() { return _write_buffer; }

std::string Client::getRealname() const { return _realname; }

const std::map<std::string, Channel*>& Client::getJoinedChannels() { return _joinedChannels; }

const std::vector<Channel*> &Client::getInvitedChannels() const { return _invitedChannels; }

// Setters
void Client::setIp(const std::string& ip) { _ip = ip; }

void Client::setNickname(const std::string& nickname) { _nickname = nickname; }

void Client::setUsername(const std::string& username) { _username = username; }

void Client::setRealname(const std::string& realname) { _realname = realname; }

void Client::setRegistered(bool status) { _is_registered = status; }

void Client::setPasswordVerified(bool status) { _is_password_verified = status; }


//JoinedChannels
void Client::addJoinedChannels(const std::string& name, Channel* channel)
{
	_joinedChannels[name] = channel;
}

void Client::removeJoinedChannels(const std::string& name)
{
	_joinedChannels.erase(name);
}

// InvitedChannels
void Client::addInvitedChannel(Channel *ch)
{
	_invitedChannels.push_back(ch);
}

void Client::removeInvitedChannel(Channel *ch)
{
	_invitedChannels.erase(std::remove(_invitedChannels.begin(), _invitedChannels.end(), ch), _invitedChannels.end());
}

// Other Methods
void Client::sendReply(const std::string& message)
{
	_write_buffer += message + "\r\n";
}

bool Client::hasCompleteLine() const { return _read_buffer.find('\n') != std::string::npos; }

bool Client::hasPasswordVerified() const { return _is_password_verified; }

bool Client::isRegistered() const { return _is_registered; }

std::string Client::popLine()
{
	size_t pos = _read_buffer.find('\n');
	std::string line = _read_buffer.substr(0, pos);
	if (!line.empty() && line[line.size() - 1] == '\r')
		line.erase(line.size() - 1);
	_read_buffer.erase(0, pos + 1);
	return line;
}

void Client::updateLastActivity() { _lastActivity = std::time(NULL); _pingSent = false; }
long Client::getIdleTime() const { return static_cast<long>(std::difftime(std::time(NULL), _lastActivity)); }
long Client::getConnectTime() const { return static_cast<long>(std::difftime(std::time(NULL), _connectedAt)); }

bool Client::isPingSent() const { return _pingSent; }
void Client::setPingSent(bool status) { _pingSent = status; }
void Client::markClosing() { _closingSince =  std::time(NULL); }
long Client::getClosingTime() const
{
	if (_closingSince == 0)
		return 0;
	return (static_cast<long>(std::difftime(std::time(NULL), _closingSince)));
}

// 10/02: 환영 메시지는 001~004 전부 필수라고 제안받음.
void Client::tryCompleteRegistration()
{
	if (!isRegistered() && hasPasswordVerified() && !getNickname().empty() && !getUsername().empty())
	{
		_is_registered = true;
		sendReply(IrcReply::formatReply(IrcNumeric::RPL_WELCOME, getNickname(),
			"Welcome to the ircserv Network, " + getNickname() + "!" + getUsername() + "@" + getIp()));
		sendReply(IrcReply::formatReply(IrcNumeric::RPL_YOURHOST, getNickname(),
			"Your host is ircserv, running version 1.0"));
		sendReply(IrcReply::formatReply(IrcNumeric::RPL_CREATED, getNickname(),
			"This server was created for ircserv"));
		// RFC2812 5.1: 004는 trailing 없이 공백 구분 4개 필드, 사용자모드 미지원.
		// "o"는 자리값, 채널 모드는 itkol 지원
		sendReply(":ircserv " + IrcNumeric::RPL_MYINFO + " " + getNickname() +
			" ircserv 1.0 o itkol");
	}
}

bool Client::isMissingPassword() const
{
	return (!_is_registered && !_is_password_verified
			&& !_nickname.empty() && !_username.empty());
}

void Client::broadcastNickChange(const std::string &notice)
{
	sendReply(notice);

	std::set<Client *> notified;
	notified.insert(this);

	for (std::map<std::string, Channel*>::const_iterator it = _joinedChannels.begin();
		it != _joinedChannels.end(); ++it) {
			const std::vector<Client *> &members = it->second->getMembers();
			for (size_t idx = 0; idx < members.size(); ++idx) {
				if (notified.insert(members[idx]).second)
					members[idx]->sendReply(notice);
			}
	}
}

void Client::broadcastQuit(const std::string &notice) {
	std::set<Client *> notified;
	notified.insert(this);

	for (std::map<std::string, Channel*>::const_iterator it = _joinedChannels.begin();
		it != _joinedChannels.end(); ++it) {
			const std::vector<Client *> &members = it->second->getMembers();
			for (size_t idx = 0; idx < members.size(); ++idx) {
				if (notified.insert(members[idx]).second)
					members[idx]->sendReply(notice);
			}
	}
}
