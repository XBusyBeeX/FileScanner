#ifndef ARGSHANDLER_H
#define ARGSHANDLER_H



#include <vector>
#include <string>
#include <map>
#include <functional>

class ArgsHandlers
{
private:
	struct argHandler
	{
		std::string desctiption;
		std::function<void(const std::string&)> func;
	};

public:
	ArgsHandlers(int argc, char* argv[]);

	void insert(
		const char* command,
		const char* description,
		const std::function<void(const std::string&)>& lambda)
	{
		insert(
			std::string(command),
			std::string(description),
			lambda
		);
	}

	void insert(
		const std::string& command,
		const std::string& description,
		const std::function<void(const std::string&)>& lambda);

	void exit(int code) {
		std::exit(code);
	}

	void setReturnCode(int code) {
		m_return = code;
	}

	int exec();

private:
	int m_return = 0;
	std::vector<std::string> m_args;
	std::map<std::string, argHandler> m_handlers;
};

#endif //ARGSHANDLER_H