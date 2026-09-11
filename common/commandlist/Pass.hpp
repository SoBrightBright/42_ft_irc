#ifndef PASS_HPP
# define PASS_HPP

# include "ICommand.hpp"

	class	Pass : public ICommand {
		private:
			Pass(const Pass &obj);
			Pass	&operator=(const Pass &obj);

		public:
			Pass();
			virtual ~Pass();

			virtual bool	needLogin() const;
			virtual void	execute(Server &server, Client &client, const Message &msg);
	};

#endif