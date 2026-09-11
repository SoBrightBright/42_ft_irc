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
			std::string				 _trailing;
			bool					 existTrailing;

			void	setPrefix(const std::string &prefix);
			void	setCommand(const std::string &command);
			void	addParameter(const std::string &parameter);	// parameter는 이어붙여야하니까.
			void	setTrailing(const std::string &trailing);

			Message(const Message &obj);
			Message	&operator=(const Message &obj);

		public:
			Message();
			~Message();

			const std::string				&getPrefix() const;
			const std::string				&getCommand() const;
			const std::vector<std::string> 	&getParameter() const;
			const std::string				&getTrailing() const;

			bool	hasPrefix() const;
			bool	hasTrailing() const;

			bool	spliter(const std::string &raw);
	};

#endif