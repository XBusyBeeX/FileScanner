#ifndef CUSTOMQUEUE_H
#define CUSTOMQUEUE_H

#include <queue>
#include <atomic>

template<typename T>
class custom_queue
{
public:
	void push(const T& v) {

        acquire();
        m_queue.push(v);
        release();
	}

    T pop() {
        acquire();
        T v = !m_queue.empty() ?
            m_queue.front() :
            T();
        m_queue.pop();
        release();
        return v;
    }

    bool empty() {
        acquire();
        bool r = m_queue.empty();
        release();
        return r;
    }

private:
    void acquire()
    {
        while (m_lock.test_and_set(std::memory_order_acquire)) {  // acquire lock
// Since C++20, it is possible to update atomic_flag's
// value only when there is a chance to acquire the lock.
// See also: https://stackoverflow.com/questions/62318642
#if defined(__cpp_lib_atomic_flag_test)
            while (m_lock.test(std::memory_order_relaxed))        // test lock
#endif
                ; // spin
        }
    }

    void release() {
        m_lock.clear(std::memory_order_release);                  // release lock
    }

private:
	std::queue<T> m_queue;
	std::atomic_flag m_lock = ATOMIC_FLAG_INIT;
};

#endif // CUSTOMQUEUE_H