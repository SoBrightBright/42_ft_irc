#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include "../Client.hpp"
#include "../common_ch/Reply.hpp" // TODO: 수정할 것

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
		void	handleModeOperator(bool enable, Client &target, Client &executor);
		void	handleChannelPrivmsg(Client &sender, const std::string &message);

		// getters
		const std::string 	&getName() const;
		const std::string	&getTopic() const;

		// checkers
		bool				isMember(const Client &client) const;
		bool				isOperator(const Client &client) const;		
};

#endif