#ifndef PRIVMSG_HPP
# define PRIVMSG_HPP

# include "ICommand.hpp"

	class	Privmsg : public ICommand {
		private:
			Privmsg(const Privmsg &obj);
			Privmsg	&operator=(const Privmsg &obj);

		public:
			Privmsg();
			virtual ~Privmsg();

			virtual bool	needLogin() const;
			virtual void	execute(Server &server, Client &client, const Message &msg);
	};

#endif