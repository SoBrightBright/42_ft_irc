#ifndef KICK_HPP
# define KICK_HPP

# include "ICommand.hpp"

	class	Kick : public ICommand {
		private:
			Kick(const Kick &obj);
			Kick	&operator=(const Kick &obj);

		public:
			Kick();
			virtual ~Kick();

			virtual bool	needLogin() const;
			virtual void	execute(Server &server, Client &client, const Message &msg);
	};

#endif