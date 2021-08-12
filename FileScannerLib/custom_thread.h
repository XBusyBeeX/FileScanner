#ifndef CUSTOMTHREAD_H
#define CUSTOMTHREAD_H

#include <thread>
#include <atomic>
class custom_thread
{
public:
	void start();
	bool running();
protected:
	~custom_thread();

	virtual void run() {}


private:
	std::thread *m_thr = nullptr;
	std::atomic_bool m_isRunning = false;
};

#endif // CUSTOMTHREAD_H
