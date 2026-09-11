#ifndef NICK_HPP
# define NICK_HPP

# include "ICommand.hpp"

	class	Nick : public ICommand {
		private:
			Nick(const Nick &obj);
			Nick	&operator=(const Nick &obj);

		public:
			Nick();
			virtual ~Nick();

			virtual bool	needLogin() const;
			virtual void	execute(Server &server, Client &client, const Message &msg);
	};

#endif