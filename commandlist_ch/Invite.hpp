#ifndef INVITE_HPP
# define INVITE_HPP

# include "ICommand.hpp" // 합치면 경로 문제 사라질 예정

	class	Invite : public ICommand {
		private:
			Invite(const Invite &obj);
			Invite	&operator=(const Invite &obj);

		public:
			Invite();
			virtual ~Invite();

			virtual bool	needLogin() const;
			virtual void	execute(Server &server, Client &client, const Message &msg);
	};

#endif