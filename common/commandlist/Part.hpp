#ifndef PART_HPP
# define PART_HPP

# include "ICommand.hpp"

	class	Part : public ICommand {
		private:
			Part(const Part &obj);
			Part	&operator=(const Part &obj);

		public:
			Part();
			virtual ~Part();

			virtual bool	needLogin() const;
			virtual void	execute(Server &server, Client &client, const Message &msg);
	};

#endif