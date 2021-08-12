#include "FileMapping.h"
#include <Windows.h>

FileMapping::FileMapping(const std::string& filename, const unsigned long long& size)
{
	setFileName(filename);
}

FileMapping::~FileMapping()
{
	close();
}

void FileMapping::setFileName(const std::string& filename)
{
	close();
	m_filename = filename;
}

void FileMapping::setFileSize(const unsigned long long& size)
{
	m_size = size;
}

bool FileMapping::open(const DesiredAccess& access)
{
	close();

	m_handle = CreateFileA(
					m_filename.c_str(),
					GENERIC_READ,
					0,
					nullptr,
					OPEN_ALWAYS,
					FILE_ATTRIBUTE_NORMAL,
					nullptr
				);

	if (!m_handle || m_handle == INVALID_HANDLE_VALUE)
	{
		m_handle = nullptr;
		close();
		return false;
	}
		

	if (!m_size)
	{
		if ((m_size = GetFileSize(m_handle, LPDWORD(PCHAR(&m_size) + 32)))
			== INVALID_FILE_SIZE)
		{
			m_size = 0;
		}
	}

	m_access = access;

	return true;
}

bool FileMapping::mapping()
{
	if (!m_size)
	{
		unmap();
		return false;
	}
	
		unsigned long page_access = 0;

		{
			if (m_access == DesiredAccess::all)
				page_access = PAGE_EXECUTE_READWRITE;
			else if (
				(unsigned long(m_access) & unsigned long(DesiredAccess::execute))
				)
			{
				page_access = PAGE_EXECUTE;

				if ((unsigned long(m_access) & unsigned long(DesiredAccess::write)))
					page_access = PAGE_EXECUTE_READWRITE;
			}
			else if (
				(unsigned long(m_access) & unsigned long(DesiredAccess::read))
				)
			{
				page_access = PAGE_READONLY;

				if ((unsigned long(m_access) & unsigned long(DesiredAccess::write)))
					page_access = PAGE_READWRITE;
			}
		}

		m_handleMap = CreateFileMappingA(
			m_handle,
			nullptr,
			page_access,
			DWORD(m_size >> 32),
			DWORD(m_size),
			nullptr
		);

		if (!m_handleMap || m_handleMap == INVALID_HANDLE_VALUE)
		{
			m_handleMap = nullptr;
			unmap();
			return false;
		}

	return true;
}

bool FileMapping::view(const unsigned long long& offset, const unsigned long& size)
{
	if (!size)
		return false;

	unsigned long sz = size;

	if (m_size - offset < UINT_MAX)
		sz = unsigned long(m_size - offset);

	m_mapAddress = MapViewOfFile(
		m_handleMap,
		FILE_MAP_READ,
		DWORD(offset>>32),
		DWORD(offset),
		sz
	);

	if (!m_mapAddress)
	{
		unview();
		return false;
	}
		

	return true;
}

void FileMapping::unview()
{
	if (m_mapAddress)
		UnmapViewOfFile(m_mapAddress);

	m_mapAddress = nullptr;
}

void FileMapping::unmap()
{
	if (m_handleMap)
		CloseHandle(m_handleMap);

	m_handleMap = nullptr;
}

void FileMapping::close()
{	
	unview();
	unmap();

	if(m_handle)
		CloseHandle(m_handle);

	m_handle = nullptr;
}
