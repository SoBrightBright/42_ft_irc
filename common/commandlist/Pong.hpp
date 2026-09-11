#ifndef PONG_HPP
# define PONG_HPP

# include "ICommand.hpp"

	class	Pong : public ICommand {
		private:
			Pong(const Pong &obj);
			Pong	&operator=(const Pong &obj);

		public:
			Pong();
			virtual ~Pong();

			virtual bool	needLogin() const;
			virtual void	execute(Server &server, Client &client, const Message &msg);
	};

#endif