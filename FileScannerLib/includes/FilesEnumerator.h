#ifndef FILESENUMERATOR_H
#define FILESENUMERATOR_H

#include "../custom_thread.h"
#include "../custom_queue.h"
#include <string>
#include <atomic>

struct FilesEnumeratorResult
{
	FilesEnumeratorResult() : size(0) {}

	std::string path;
	uint64_t size;
};

class FilesEnumerator : private custom_thread, private custom_queue<FilesEnumeratorResult>
{

public:
	void append(const std::string& path) { m_dirs.push(path); }
	FilesEnumeratorResult get() { return !empty() ? pop() : FilesEnumeratorResult(); }

	void start() { m_running.store(true); static_cast<custom_thread*>(this)->start(); }

	bool running() { return m_running.load(); }

	bool isEmpty() { return empty(); }

private:
	void enumerate(const std::string &);

	void run() override;


private:
	custom_queue<std::string> m_dirs;
	std::atomic_bool m_running = false;

};

#endif // FILESENUMERATOR_H