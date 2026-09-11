#ifndef PING_HPP
# define PING_HPP

# include "ICommand.hpp"

	class	Ping : public ICommand {
		private:
			Ping(const Ping &obj);
			Ping	&operator=(const Ping &obj);

		public:
			Ping();
			virtual ~Ping();

			virtual bool	needLogin() const;
			virtual void	execute(Server &server, Client &client, const Message &msg);
	};

#endif