#ifndef JOIN_HPP
# define JOIN_HPP

# include "ICommand.hpp"

	class	Join : public ICommand {
		private:
			Join(const Join &obj);
			Join	&operator=(const Join &obj);

		public:
			Join();
			virtual ~Join();

			virtual bool	needLogin() const;
			virtual void	execute(Server &server, Client &client, const Message &msg);
	};

#endif