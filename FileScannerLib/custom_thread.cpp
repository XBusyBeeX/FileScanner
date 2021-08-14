#include "custom_thread.h"

void custom_thread::start()
{
	if (m_thr && m_isRunning.load())
		return;

	if(m_thr)
		delete m_thr;

	m_thr = new std::thread([this]()
		{
			m_isRunning.store(true);
			run();
			m_isRunning.store(false);
		});
}

bool custom_thread::running()
{
	return m_isRunning.load();
}


custom_thread::~custom_thread()
{
	if (m_thr)
	{
		while (running());
		m_thr->detach();
		delete m_thr;
	}
		
}
