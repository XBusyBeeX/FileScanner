#ifndef FILESSCANNER_H
#define FILESSCANNER_H

#include <string>
#include "../FileMapping.h"

#include "FilesEnumerator.h"
#include "../custom_thread.h"
#include "../custom_queue.h"

struct FilesScannerResult
{
    FilesScannerResult() : crc(0) {}

    std::string path;
    unsigned int crc;
};

class FilesScanner : private custom_thread, private custom_queue<FilesScannerResult>
{
public:
    FilesScanner() {}

    FilesScannerResult scan(const FilesEnumeratorResult& fenumResult);

    void start() { 
        if (m_running)
            return;

        m_waiting.store(false); 
        m_running.store(true); 
        static_cast<custom_thread*>(this)->start();
    }

    void stop() { 
        if (!m_running.load())
            return;

        m_waiting.store(true); 
        m_running.store(false); 
    }

    bool running() { return m_running.load(); }
    bool waiting() { return m_waiting.load(); }

    void append(const FilesEnumeratorResult& fenumResult) { m_qfenum.push(fenumResult); }
    FilesScannerResult get() { return !empty() ? pop() : FilesScannerResult(); }
    bool isEmpty() { return empty(); }
public:
    static unsigned int crc32(unsigned char* ptr, size_t len);

private:
    void run() override;

private:
    FileMapping m_fileMap;
    custom_queue<FilesEnumeratorResult> m_qfenum;
    std::atomic_bool m_running = false;
    std::atomic_bool m_waiting = false;
};

#endif // FILESSCANNER_H