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

bool Client::isAuthenticated() const { return _is_authenticated; }

std::string& Client::getReadBuffer() { return _read_buffer; }

std::string& Client::getWriteBuffer() { return _write_buffer; }

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