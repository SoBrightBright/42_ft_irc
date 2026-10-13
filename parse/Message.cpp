#include "Message.hpp"

#include <cctype>

Message::Message() : existPrefix(false) {}
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

const std::string	&Message::getPrefix() const { return _prefix; }
const std::string 	&Message::getCommand() const { return _command; }
const std::vector<std::string> 	&Message::getParameter() const { return _parameter; }

bool	Message::hasPrefix() const { return existPrefix; }

// RFC에서는 파싱 실패 시의 대응을 규정하지 않고,
// 실무적 관점에서 사소한 오류는 관대하게 무시하는 편.
// 안정성 요구사항과 실전 리스크 대응 측면에서 파싱오류는 true/false 처리로만 마무리
bool	Message::spliter(const std::string &raw) {
	// RFC2812 2.3: CR-LF 포함 512byte. raw는 \r\n을 제외하고 들어오므로 510byte.
	if (raw.empty())
		return false;

	// handle UTF-8 characters (e.g. 한글)
	std::string	searing = raw;
	if (searing.size() > 510) {
		size_t	cut = 510;
		for (int back = 0; back < 3 && (static_cast<unsigned char>(searing[cut]) & 0xC0) == 0x80; ++back)
			--cut;
		searing.erase(cut);
	}

	if (searing.find('\0') != std::string::npos)
		return false;

	size_t rpos = searing.find('\r');
	if (rpos != std::string::npos && rpos != searing.size() - 1)
		return false;

	std::string	seared;
	if (rpos == searing.size() - 1)
		seared = searing.substr(0, rpos);
	else
		seared = searing;

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

	// RFC2812 2.3.1: middle과 trailing은 의미상 동등.
	// RFC2812 2.3.1: middle이 이미 14개라면 15번째부터는 ':'가 없어도 trailing.
	size_t	paramCount = 0;
	while (pos < searedSize) {
		if (seared[pos] == ':' || paramCount == 14) {
			std::string	trail;
			if (seared[pos] == ':')
				trail = seared.substr(pos + 1);
			else
				trail = seared.substr(pos);
			addParameter(trail);
			++paramCount;
			break ;
		}

		end = seared.find(' ', pos);
		if (end == std::string::npos) {
			addParameter(seared.substr(pos));
			break;
		}
		addParameter(seared.substr(pos, end - pos));
		++paramCount;
		pos = end;
		while (pos < searedSize && seared[pos] == ' ')
			++pos;
	}
	return true;
}
