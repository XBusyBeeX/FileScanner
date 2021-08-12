#ifndef FILEMAPPING_H
#define FILEMAPPING_H

#include <string>


class FileMapping
{
public:
	enum class DesiredAccess
	{
		read =		(0x80000000L),
		write =		(0x40000000L),
		execute =	(0x20000000L),
		all =		(0x10000000L)
	};

public:
	FileMapping() {}
	FileMapping(const std::string &filename, const unsigned long long &size = 0);
	~FileMapping();

	void setFileName(const std::string& filename);
	void setFileSize(const unsigned long long& size = 0);
	bool open(const DesiredAccess &access);

	bool mapping();
	bool view(const unsigned long long &offset = 0, const unsigned long &size = UINT_MAX);

	void unview();
	void unmap();
	void close();

	void* data() { return m_mapAddress; }
	unsigned long long size() const { return m_size; }
private:
	std::string m_filename;

	void* m_handle = nullptr;
	void* m_handleMap = nullptr;
	void* m_mapAddress = nullptr;

	unsigned long long m_size = 0;

	DesiredAccess m_access;
};

#endif //FILEMAPPING_H