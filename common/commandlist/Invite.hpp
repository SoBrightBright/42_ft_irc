#ifndef INVITE_HPP
# define INVITE_HPP

# include "ICommand.hpp"

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