#include "Client.hpp"

#include <string>

Client::Client(int fd):
	_fd(fd), _nickname(""), _username(""), _is_authenticated(false),
	_is_registered(false), _read_buffer(""), _write_buffer("") {}

Client::Client(const Client& other)
{
	_fd = other._fd;
	_nickname = other._nickname;
	_username = other._username;
	_is_authenticated = other._is_authenticated;
	_is_registered = other._is_registered;
	_read_buffer = other._read_buffer;
	_write_buffer = other._write_buffer;
}

Client &Client::operator=(const Client& other)
{
	if (this != &other)
	{
		_fd = other._fd;
		_nickname = other._nickname;
		_username = other._username;
		_is_authenticated = other._is_authenticated;
		_is_registered = other._is_registered;
		_read_buffer = other._read_buffer;
		_write_buffer = other._write_buffer;
	}
	return *this;
}

Client::~Client() {}

int Client::getFd() const { return _fd; }

std::string Client::getNickname() const { return _nickname; }

std::string Client::getUsername() const { return _username; }

bool Client::isRegistered() const { return _is_registered; }

bool Client::isAuthenticated() const { return _is_authenticated; }

void Client::sendReply(const std::string& message)
{
	_write_buffer += message + "\r\n";
}

std::string& Client::getReadBuffer() { return _read_buffer; }

std::string& Client::getWriteBuffer() { return _write_buffer; }

std::string Client::getIp() const { return _ip; }

void Client::setNickname(const std::string& nickname) { _nickname = nickname; }

void Client::setUsername(const std::string& username) { _username = username; }

void Client::setAuthenticated(bool status) { _is_authenticated = status; }

void Client::setRegistered(bool status) { _is_registered = status; }

void Client::setIp(const std::string& ip) { _ip = ip; }

bool Client::hasCompleteLine() const { return _read_buffer.find('\n') != std::string::npos; }

std::string Client::popLine()
{
	size_t pos = _read_buffer.find('\n');
	std::string line = _read_buffer.substr(0, pos);
	if (!line.empty() && line[line.size() - 1] == '\r')
		line.erase(line.size() - 1);
	_read_buffer.erase(0, pos + 1);
	return line;
}