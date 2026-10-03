#ifndef MESSAGE_HPP
# define MESSAGE_HPP

	# include <string>
	# include <vector>

	class	Message {
		private:
			std::string				 _prefix;
			bool					 existPrefix;
			std::string				 _command;
			std::vector<std::string> _parameter;

			void	setPrefix(const std::string &prefix);
			void	setCommand(const std::string &command);
			void	addParameter(const std::string &parameter);	// parameter는 이어붙여야하니까.

			Message(const Message &obj);
			Message	&operator=(const Message &obj);

		public:
			Message();
			~Message();

			const std::string				&getPrefix() const;
			const std::string				&getCommand() const;
			const std::vector<std::string> 	&getParameter() const;

			bool	hasPrefix() const;

			bool	spliter(const std::string &raw);
	};

#endif