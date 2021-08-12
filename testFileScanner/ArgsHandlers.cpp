#include "ArgsHandlers.h"

ArgsHandlers::ArgsHandlers(int argc, char* argv[])
{
	for (int i = 1; i < argc; i++)
		m_args.push_back(std::string(argv[i]));
}

void ArgsHandlers::insert(
	const std::string& command,
	const std::string& description,
	const std::function<void(const std::string&)>& lambda)
{
	argHandler h;
	h.desctiption = description;
	h.func = lambda;

	m_handlers[command] = h;
}

int ArgsHandlers::exec()
{
	argHandler h;

	for (size_t i=0;i<m_args.size();i++)
	{
		const std::string& k = m_args[i];

		if (m_handlers.count(k))
		{
			h = m_handlers.at(k);

			if (h.func && ++i < m_args.size())
				h.func(m_args[i]);
		}
	}

	return m_return;
}
