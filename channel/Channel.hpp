#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include "../Client.hpp"
#include "Reply.hpp"

class Client;

class Channel
{
	private:
		// basic settings
		std::string					_name;
		std::string					_topic;
		std::string					_key;			// +k의 key
		int							_userLimit;		// +l의 user limit. 0이나 -1이면 제한 없음

		// user lists
		std::vector<Client *> 		_members;
		std::vector<Client *>		_operators;
		std::vector<std::string>	_invitedUserNicks;

		// permission
		bool						_inviteOnly;	// +i
		bool						_topicByOpOnly;	// +t

		// checkers
		bool		checkKey(const std::string &key) const;
		bool		isFull() const;
		bool		isInvited(const Client &client) const;
		
		// member & operator administration
		void		addMember(Client &client);
		void		removeMember(Client &client);
		void		addOperator(Client &client);
		void		removeOperator(Client &client);

		// changing mode flags
		void		setModeInviteOnly(bool enable);						// i
		void		setModeTopicRestriction(bool enable);				// t
		void		setModeKey(bool enable, const std::string &key);	// k
		void		setModeOperator(bool enable, Client &user);			// o
		void		setModeUserLimit(bool enable, int userLimit);		// l

		// sending messages
		void		sendToOne(Client &targetUser, const std::string &message) const;
		void		sendToAll(const std::string &message) const;
		void		sendToAllExcept(const Client &except, const std::string &message) const;

		std::string	buildModeString() const;

	public:
		// Orthodox Canonical Form
				Channel();
				Channel(const std::string &name);
				Channel(const Channel &other);
				Channel &operator=(const Channel &other);
				~Channel();

		// commands
		void	handleJoin(Client &user, const std::string &key);
		void	handlePart(Client &user, const std::string &message);
		void	handleKick(Client &kickingUser, Client &kickedUser, const std::string &comment);
		void	handleInvite(Client &invitingUser, Client &invitedUser);
		void	handleTopic(Client &user, const std::string &topic, bool hasTopic);
		void	handleMode(Client &user, const std::string &modes, const std::vector<std::string> &modeParams);
		void	handlePrivmsg(Client &sendingUser, Client &sentUser, const std::string &message);
		void	handleChannelPrivmsg(Client &sender, const std::string &message);

		// getters
		const std::string 	&getName() const;
		const std::string	&getTopic() const;

		// checkers
		bool				isMember(const Client &client) const;
		bool				isOperator(const Client &client) const;		
};

#endif


// solee
// 채널 객체 저장소와 새 채널 생성/삭제 담당하는 것은 누구??
// 클라이언트의 채널 소속 목록 (_joinedChannels)을 추가핧 것인지?
// // _write_buffer가 public? or setter? (reference면 안 되지 않나?)

// jimkim
// 에러코드? 파싱 측에서? 여기서?
// handleMode에서 어떤 형태로 들어올 것인지, +/-도 나눠서?판단해서? 들어오는지
// TOPIC (조회와 빈 문자열 구분)을 어떻게 줄 것인지 (bool hasTopicParam?)


// numeric reply - sendToOne/sendToAll 문자열들 코드 번호 형태로 바꾸기