#ifndef MODE_HPP
# define MODE_HPP

# include "ICommand.hpp"

	class	Mode : public ICommand {
		private:
			Mode(const Mode &obj);
			Mode	&operator=(const Mode &obj);

		public:
			Mode();
			virtual ~Mode();

			virtual bool	needLogin() const;
			virtual void	execute(Server &server, Client &client, const Message &msg);
	};

#endif