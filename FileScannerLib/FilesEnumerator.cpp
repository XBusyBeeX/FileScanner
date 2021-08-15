#include "includes/FilesEnumerator.h"
#include <filesystem>


void FilesEnumerator::enumerate(const std::string& path)
{
	FilesEnumeratorResult res;

	try
	{


		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
				if (!entry.is_directory())
				{
					res.size = entry.file_size();
					res.path = entry.path().string();
					push(res);
				}
				else
					m_dirs.push(entry.path().string());

		}
	}
	catch (const std::exception&) {}
}

void FilesEnumerator::run()
{
	m_running.store(true);

	while (!m_dirs.empty())
		enumerate(m_dirs.pop());

	m_running.store(false);
}
