#ifndef USER_HPP
# define USER_HPP

# include "ICommand.hpp"

	class	User : public ICommand {
		private:
			User(const User &obj);
			User	&operator=(const User &obj);

		public:
			User();
			virtual ~User();

			virtual bool	needLogin() const;
			virtual void	execute(Server &server, Client &client, const Message &msg);
	};

#endif