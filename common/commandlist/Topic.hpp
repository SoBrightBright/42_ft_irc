#ifndef TOPIC_HPP
# define TOPIC_HPP

# include "ICommand.hpp"

	class	Topic : public ICommand {
		private:
			Topic(const Topic &obj);
			Topic	&operator=(const Topic &obj);

		public:
			Topic();
			virtual ~Topic();

			virtual bool	needLogin() const;
			virtual void	execute(Server &server, Client &client, const Message &msg);
	};

#endif