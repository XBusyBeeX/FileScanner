#include "../FileScannerLib/includes/FilesScanner.h"

FilesScannerResult FilesScanner::scan(const FilesEnumeratorResult& fenumResult)
{
    FilesScannerResult result;

    m_fileMap.setFileName(fenumResult.path);
    m_fileMap.setFileSize(fenumResult.size);

    if (
        !m_fileMap.open(FileMapping::DesiredAccess::read) ||
        !m_fileMap.mapping() ||
        !m_fileMap.view()
        )
        return result;

    

    result.path = fenumResult.path;
    result.crc = FilesScanner::crc32(static_cast<unsigned char*>(m_fileMap.data()), size_t(m_fileMap.size()));

    m_fileMap.close();

    return result;
}

void FilesScanner::run()
{
    while (m_running.load() || m_waiting.load())
    {
        while (!m_qfenum.empty())
            push(scan(m_qfenum.pop()));

            m_waiting.store(m_running.load() || !m_qfenum.empty());
    }


}

//-----------------STATICS-------------------------------

unsigned int FilesScanner::crc32(unsigned char* ptr, size_t len)
{
    unsigned int crc_table[256];
    unsigned int crc; int i, j;

    for (i = 0; i < 256; i++)
    {
        crc = i;
        for (j = 0; j < 8; j++)
            crc = crc & 1 ? (crc >> 1) ^ 0xEDB88320UL : crc >> 1;

        crc_table[i] = crc;
    };

    crc = 0xFFFFFFFFUL;

    while (len--)
        crc = crc_table[(crc ^ *ptr++) & 0xFF] ^ (crc >> 8);

    return crc ^ 0xFFFFFFFFUL;
}


