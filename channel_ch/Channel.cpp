#include "Channel.hpp"

////// Orthodox Canonical Form //////

Channel::Channel()
: _name("default"), _topic(""), _key(""), _userLimit(0), _inviteOnly(false), _topicByOpOnly(false)
{
	// _members, _operators, _invitedUserNicks는 언급 안 해도
	// 자동으로 std::vector<Client*>()가 호출돼서 크기 0짜리 빈 벡터로 초기화됨
	// 반면 int는 쓰레기 값으로 남음
	// std::string, std::vector 같은 클래스 타입은 값이 없을 때 기본으로 뭘로 시작할지를 스스로 이미 정의해두고 있어서 아무것도 안 써둬도 안전하게 초기화 가능.
}

Channel::~Channel()
{

}

Channel::Channel(const std::string &name)
: _name(name), _topic(""), _key(""), _userLimit(0), _inviteOnly(false), _topicByOpOnly(false)
{

}

Channel::Channel(const Channel &other)
{
	*this = other;
}

Channel &Channel::operator=(const Channel &other)
{
	if (this != &other)
	{
		_name = other._name;
		_topic = other._topic;
		_key = other._key;
		_userLimit = other._userLimit;
		_inviteOnly = other._inviteOnly;
		_topicByOpOnly = other._topicByOpOnly;
		_members = other._members; // std::vector도 =를 그냥 쓸 수 있음: vector 자체가 대입 연산자를 이미 가지고 있음. (string 대입처럼)
		_operators = other._operators;
		_invitedUserNicks = other._invitedUserNicks;
	}
	return *this;
}

//////////////////////////////////////////////////

const std::string 	&Channel::getName() const
{
	return _name;
}

const std::string	&Channel::getTopic() const
{
	return _topic;
}

bool	Channel::checkKey(const std::string &key) const
{
	return (_key == key);
}

bool	Channel::isFull() const
{
	if (_userLimit <= 0) // 제한 없을 경우
		return false;
	return (_members.size() >= static_cast<size_t>(_userLimit));
}

bool	Channel::isInvited(const Client &client) const
{
	for (size_t i = 0; i < _invitedUserNicks.size(); i++)
	{
		if (_invitedUserNicks[i] == client.getNickname())
			return true;
	}
	return false;
}

bool	Channel::isMember(const Client &client) const
{
	for (size_t i = 0; i < _members.size(); i++)
	{
		if (_members[i] == &client) // &client가 가리키는 객체의 주소를 꺼내서 포인터끼리 비교 (같은 메모리 주소를 가리키는가?)
			return true;
	}
	return false;
}

bool	Channel::isOperator(const Client &client) const
{
	for (size_t i = 0; i < _operators.size(); i++)
	{
		if (_operators[i] == &client)
			return true;
	}
	return false;
}

void	Channel::addMember(Client &client)
{
	if (!isMember(client))
		_members.push_back(&client);
}

void	Channel::removeMember(Client &client)
{
	_members.erase(std::remove(_members.begin(), _members.end(), &client), _members.end());
}

void	Channel::addOperator(Client &client)
{
	if (!isOperator(client))
		_operators.push_back(&client);
}

void	Channel::removeOperator(Client &client)
{
	_operators.erase(std::remove(_operators.begin(), _operators.end(), &client), _operators.end());
}

void	Channel::setModeInviteOnly(bool enable) // i
{
	_inviteOnly = enable;
}

void	Channel::setModeTopicRestriction(bool enable) // t
{
	_topicByOpOnly = enable;
}

void	Channel::setModeKey(bool enable, const std::string &key) // k
{
	if (enable)
		_key = key;
	else
		_key = "";
}

void	Channel::setModeOperator(bool enable, Client &user)	// o
{
	if (enable)
	{
		if (isOperator(user))
			return ;
		_operators.push_back(&user);
	}
	else
	{
		if (!isOperator(user))
			return ;
		_operators.erase(std::remove(_operators.begin(), _operators.end(), &user), _operators.end());
	}
}

void	Channel::setModeUserLimit(bool enable, int userLimit) // l
{
	if (enable)
		_userLimit = userLimit;
	else
		_userLimit = 0;
}

////// Commands //////

void	Channel::handleJoin(Client &user, const std::string &key)
{
	if (!_key.empty() && !checkKey(key))
	{
		sendToOne(user, makeReply(Numeric::ERR_BADCHANNELKEY, user.getNickname(), _name + " :Cannot join channel (+k)"));
		return ;
	}
	if (_inviteOnly && !isInvited(user))
	{
		sendToOne(user, makeReply(Numeric::ERR_INVITEONLYCHAN, user.getNickname(), _name + " :Cannot join channel (+i)"));
		return ;
	}
	if (isFull())
	{
		sendToOne(user, makeReply(Numeric::ERR_CHANNELISFULL, user.getNickname(), _name + " :Cannot join channel (+l)"));
		return ;
	}

	addMember(user);
	sendToAll(makeCommand(user, "JOIN", _name));
	if (_topic.size() == 0)
		sendToOne(user, makeReply(Numeric::RPL_NOTOPIC, user.getNickname(), _name + " :No topic is set"));
	else
		sendToOne(user, makeReply(Numeric::RPL_TOPIC, user.getNickname(), _name + " :" + _topic));
	std::string users;	
	for (size_t i = 0; i < _members.size(); i++)
	{
		users += _members[i]->getNickname();
		if (i + 1 < _members.size())
			users += ", ";
	}
	sendToOne(user, makeReply(Numeric::RPL_NAMREPLY, user.getNickname(), "= " + _name + " :" + users));
	sendToOne(user, makeReply(Numeric::RPL_ENDOFNAMES, user.getNickname(), _name + " :End of /NAMES list"));
}

void	Channel::handlePart(Client &user, const std::string &message)
{
	if (!isMember(user))
	{
		sendToOne(user, makeReply(Numeric::ERR_NOTONCHANNEL, user.getNickname(), _name + " :You're not on that channel"));
		return ;
	}
	
	std::string reason;
	if (message.size() > 0)
		reason = message;
	else
		reason = user.getNickname();

	removeMember(user);

	sendToAll(makeCommand(user, "PART", _name + " :" + reason));
}

void	Channel::handleKick(Client &kickingUser, Client &kickedUser, const std::string &comment)
{
	if (!isMember(kickingUser))
	{
		sendToOne(kickingUser, makeReply(Numeric::ERR_NOTONCHANNEL, kickingUser.getNickname(), _name + " :You're not on that channel"));
		return ;
	}
	if (!isOperator(kickingUser))
	{
		sendToOne(kickingUser, makeReply(Numeric::ERR_CHANOPRIVSNEEDED, kickingUser.getNickname(), _name + " :You're not channel operator"));
		return ;
	}
	if (!isMember(kickedUser))
	{
		sendToOne(kickingUser, makeReply(Numeric::ERR_USERNOTINCHANNEL, kickingUser.getNickname(), kickedUser.getNickname() + " " + _name + " :They aren't on that channel"));
		return ;
	}

	std::string reason;
	if (!comment.empty())
		reason = comment;
	else
		reason = kickingUser.getNickname();

	removeMember(kickedUser);

	sendToAll(makeCommand(kickingUser, "KICK", _name + " " + kickedUser.getNickname()));
}

void	Channel::handleTopic(Client &user, const std::string &topic, bool hasTopicParam)
{
	if (!isMember(user))
	{
		sendToOne(user, makeReply(Numeric::ERR_NOTONCHANNEL, user.getNickname(), _name + " :You're not on that channel"));
		return ;
	}
	if (!hasTopicParam)
	{
		if (_topic.empty())
			sendToOne(user, makeReply(Numeric::RPL_NOTOPIC, user.getNickname(), _name + " :No topic is set"));
		else
			sendToOne(user, makeReply(Numeric::RPL_TOPIC, user.getNickname(), _name + " :" + _topic));
		return ;
	}
	if (_topicByOpOnly && !isOperator(user))
	{
		sendToOne(user, makeReply(Numeric::ERR_CHANOPRIVSNEEDED, user.getNickname(), _name + " :You're not channel operator"));
		return ;
	}
	_topic = topic;
	sendToAll(makeCommand(user, "TOPIC", _name + " :" + _topic));
}

void	Channel::handleInvite(Client &invitingUser, Client &invitedUser)
{
	if (!isMember(invitingUser))
	{
		sendToOne(invitingUser, makeReply(Numeric::ERR_NOTONCHANNEL, invitingUser.getNickname(), _name + " :You're not on that channel"));
		return ;
	}
	if (_inviteOnly && !isOperator(invitingUser))
	{
		sendToOne(invitingUser, makeReply(Numeric::ERR_CHANOPRIVSNEEDED, invitingUser.getNickname(), _name + " :You're not channel operator"));
		return ;
	}
	if (isInvited(invitedUser))
	{
		return ; // 이미 초대된 상태 - RFC가 별도 에러 코드를 정의하지 않아 조용히 무시
	}
	if (isMember(invitedUser))
	{
		sendToOne(invitingUser, makeReply(Numeric::ERR_USERONCHANNEL, invitingUser.getNickname(), invitedUser.getNickname() + " " + _name + " :is already on channel"));
		return ;
	}
	_invitedUserNicks.push_back(invitedUser.getNickname());
	sendToOne(invitingUser, makeReply(Numeric::RPL_INVITING, invitingUser.getNickname(), _name + " " + invitedUser.getNickname()));
	sendToOne(invitedUser, makeCommand(invitingUser, "INVITE", invitedUser.getNickname() + " " + _name));
}

std::string	Channel::buildModeString() const
{
	std::string	modes = "+";
	std::string params;

	if (_inviteOnly)
		modes += "i";
	if (_topicByOpOnly)
		modes += "t";
	if (!_key.empty())
	{
		modes += "k";
		params += " " + _key;
	}
	if (_userLimit > 0)
	{
		modes += "l";
		std::ostringstream oss;
		oss << _userLimit;
		params += " " + oss.str();
	}
	return modes + params;
}

void	Channel::handleMode(Client &user, const std::string &modes, const std::vector<std::string> &modeParams)
{
	if (modes.empty())
	{
		sendToOne(user, makeReply(Numeric::RPL_CHANNELMODEIS, user.getNickname(), _name + " " + buildModeString()));
		return ;
	}
	if (!isOperator(user))
	{
		sendToOne(user, makeReply(Numeric::ERR_CHANOPRIVSNEEDED, user.getNickname(), _name + " :You're not channel operator"));
		return ;
	}

	bool	enable = false;
	size_t	paramIndex = 0;

	for (size_t i = 0; i < modes.size(); i++)
	{
		char mode = modes[i];

		if (mode == '+')
		{
			enable = true;
			continue ;
		}
		if (mode == '-')
		{
			enable = false;
			continue ;
		}

		switch (mode)
		{
			case 'i':
				setModeInviteOnly(enable);
				break ;

			case 't':
				setModeTopicRestriction(enable);
				break ;

			case 'k':
				if (enable)
				{
					if (!_key.empty())
					{
						sendToOne(user, makeReply(Numeric::ERR_KEYSET, user.getNickname(), _name + " :Channel key already set"));
						break ;
					}
					if (paramIndex < modeParams.size())
						setModeKey(true, modeParams[paramIndex++]);
				}
				else
					setModeKey(false, "");
				break ;

			case 'l':
				if (enable && paramIndex < modeParams.size())
					setModeUserLimit(true, std::atoi(modeParams[paramIndex++].c_str()));
				else
					setModeUserLimit(false, 0);
				break ;

			default:
				sendToOne(user, makeReply(Numeric::ERR_UNKNOWNMODE, user.getNickname(), std::string(1, mode) + " :is unknown mode char to me for " + _name));
				break ;
		}
	}
	if (!modes.empty() && modes != "+" && modes != "-")
	{
		std::string paramsStr;
		for (size_t i = 0; i < modeParams.size(); i++)
			paramsStr += " " + modeParams[i];
		sendToAll(makeCommand(user, "MODE", _name + " " + modes + paramsStr));
	}
}

void	Channel::handleModeOperator(bool enable, Client &target, Client &user)
{
	if (!isMember(target))
	{
		sendToOne(target, makeReply(Numeric::ERR_USERNOTINCHANNEL, target.getNickname(), _name + " :They aren't on that channel"));
		return ;
	}
	setModeOperator(enable, target);
	std::string modeStr = enable ? "+o" : "-o";
	sendToAll(makeCommand(user, modeStr, _name + " " + target.getNickname()));
}

void	Channel::handleChannelPrivmsg(Client &sender, const std::string &message)
{
	if (!isMember(sender))
	{
		sendToOne(sender, makeReply(Numeric::ERR_CANNOTSENDTOCHAN, sender.getNickname(),  _name + " :Cannot send to channel"));
		return ;
	}
	sendToAllExcept(sender, makeCommand(sender, "PRIVMSG", _name + " :" + message));
}

void	Channel::sendToOne(Client &targetUser, const std::string &message) const
{
	targetUser.getWriteBuffer() += message + "\r\n";
}

void	Channel::sendToAll(const std::string &message) const
{
	for (size_t i = 0; i < _members.size(); i++)
		_members[i]->getWriteBuffer() += message + "\r\n";
}

void	Channel::sendToAllExcept(const Client &except, const std::string &message) const
{
	for (size_t i = 0; i < _members.size(); i++)
	{
		if (_members[i] != &except)
			_members[i]->getWriteBuffer() += message + "\r\n";
	}
}