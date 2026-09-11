#ifndef QUIT_HPP
# define QUIT_HPP

# include "ICommand.hpp"

	class	Quit : public ICommand {
		private:
			Quit(const Quit &obj);
			Quit	&operator=(const Quit &obj);

		public:
			Quit();
			virtual ~Quit();

			virtual bool	needLogin() const;
			virtual void	execute(Server &server, Client &client, const Message &msg);
	};

#endif