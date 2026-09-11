#include "Message.hpp"

#include <cctype>

Message::Message() : existPrefix(false), existTrailing(false) {}
Message::~Message() {}
Message::Message(const Message &obj) { (void)obj; }
Message	&Message::operator=(const Message &obj) { (void)obj; return *this; }

void	Message::setPrefix(const std::string &prefix) { 
	existPrefix = true;
	_prefix = prefix;
}

void	Message::setCommand(const std::string &command) { 
	_command = command;
	for (size_t idx = 0; idx < _command.size(); ++ idx)
		_command[idx] = std::toupper(static_cast<unsigned char>(_command[idx]));
}
void	Message::addParameter(const std::string &parameter) {
	_parameter.push_back(parameter);
}
void	Message::setTrailing(const std::string &trailing) {
	_trailing = trailing;
	existTrailing = true;
}

const std::string	&Message::getPrefix() const { return _prefix; }
const std::string 	&Message::getCommand() const { return _command; }
const std::vector<std::string> 	&Message::getParameter() const { return _parameter; }
const std::string	&Message::getTrailing() const { return _trailing; }

bool	Message::hasPrefix() const { return existPrefix; }
bool	Message::hasTrailing() const { return existTrailing;}

// RFC에서는 파싱 실패 시의 대응을 규정하지 않고,
// 실무적 관점에서 사소한 오류는 관대하게 무시하는 편.
// 안정성 요구사항과 실전 리스크 대응 측면에서 파싱오류는 true/false 처리로만 마무리
// 하지만? 필요하다면 bool -> enum화해서 별도 처리 가능.
bool	Message::spliter(const std::string &raw) {
	// RFC2812: line은 512바이트가 최대. 그런데 raw는 network 측에서 \r\n을 자르고 들어오니 510바이트가 최대.
	if (raw.empty() || raw.size() > 510)
		return false;

	if (raw.find('\0') != std::string::npos)
		return false;

	size_t rpos = raw.find('\r');
	if (rpos != std::string::npos && rpos != raw.size() - 1)
		return false;

	std::string	seared;
	if (rpos == raw.size() - 1)
		seared = raw.substr(0, rpos);
	else
		seared = raw;

	size_t	searedSize = seared.size();
	size_t	pos = seared.find_first_not_of(' ');
	size_t	end;

	if (pos == std::string::npos)
		return false;

	// get prefix
	if (seared[pos] == ':') {
		size_t	start = pos + 1;
		end = seared.find(' ', start);
		if (end == std::string::npos)
			return false;
		// ':' 에러처리 X
		if (start != end)
			setPrefix(seared.substr(start, end - start));
		pos = end;
		while (pos < searedSize && seared[pos] == ' ')
			++pos;
		if (pos >= searedSize)
			return false;
	}

	// get command
	end = seared.find(' ', pos);
	if (end == std::string::npos) {
		setCommand(seared.substr(pos));
		return true;
	}
	setCommand(seared.substr(pos, end - pos));
	pos = end;
	while (pos < searedSize && seared[pos] == ' ')
		++pos;
	if (pos >= searedSize)
		return true;

	// get parameter & trailing
	size_t	paramCount = 0;
	while (pos < searedSize) {
		if (seared[pos] == ':') {
			setTrailing(seared.substr(pos + 1));
			++paramCount;
			break ;
		}

		end = seared.find(' ', pos);
		if (end == std::string::npos) {
			addParameter(seared.substr(pos));
			++paramCount;
			break;
		}
		addParameter(seared.substr(pos, end - pos));
		++paramCount;
		pos = end;
		while (pos < searedSize && seared[pos] == ' ')
			++pos;
	}

	// parameter + trailing limit 15
	if (paramCount > 15)
		return false;
	return true;
}
