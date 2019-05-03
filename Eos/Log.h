#pragma once

#include <stdio.h>
#include <sstream>

#include "CoreDefs.h"


//#define _DEBUG    // check project settings, anyway need also this define somewhere if no prohject setting defined
#define EOS_LOG_FILENAME_MAX_LENGTH 255

EOS_NAMESPACE_BEGIN

class Log final
{
public:
    Log() :
        m_file(nullptr)
        , m_memoryAllocated(0)
        , m_memoryFreed(0)
        , m_opened(false)
    {
    }

    ~Log()
    {
    }

    void Init(const char* _fileName, eosSize _totalSize)
    {
        if (fopen_s(&m_file, _fileName, "w") != 0)
        {
            return;
        }

        m_opened = true;

        std::ostringstream os;
        os << "[BEGIN TRACE]" << std::endl << "TOTAL SIZE: " << _totalSize << std::endl << std::endl;
        fputs(os.str().c_str(), m_file);
    }

    void Shutdown()
    {
        if (!m_opened)
        {
            return;
        }

        eosS32 leak = (eosS32)m_memoryAllocated - (eosS32)m_memoryFreed;

        std::ostringstream os;
        os << "[TOTAL MEMORY ALLOCATED] " << m_memoryAllocated << std::endl << "[TOTAL MEMORY FREED] " << m_memoryFreed << std::endl << "[LEAK] " << leak << std::endl << std::endl << "[END TRACE]" << std::endl;
        fputs(os.str().c_str(), m_file);
        if (m_file != nullptr)
        {
            fclose(m_file);
        }

        m_opened = false;
    }

    void WriteAlloc(eosSize _size, eosSize _alignment, eosSize _finalSize, eosSize _chunkSize, void *_ptr)
    {
        if (!m_opened)
        {
            return;
        }

        std::ostringstream os;
        os << "[ALLOC] Pointer: " << _ptr << "; Size: " << _size << "; Alignment: " << _alignment << "; Size:" << _chunkSize << "[" << _finalSize << "]" << std::endl;
        m_memoryAllocated += _chunkSize;
        fputs(os.str().c_str(), m_file);
    }

    void WriteFree(eosSize _size, void *_ptr)
    {
        if (!m_opened)
        {
            return;
        }

        std::ostringstream os;
        os << "[FREE] Pointer: " << _ptr << "; Size: " << _size << std::endl;
        m_memoryFreed += _size;
        fputs(os.str().c_str(), m_file);
    }

    void WriteCustomMessage(const char* _msg)
    {
        if (!m_opened)
        {
            return;
        }

        fputs(_msg, m_file);
    }

    void WriteBeginRealloc()
    {
        if (!m_opened)
        {
            return;
        }

        fputs("[REALLOC]\n{\n", m_file);
    }

    void WriteEndRealloc()
    {
        if (!m_opened)
        {
            return;
        }

        fputs("}\n", m_file);
    }

private:
    eosSize m_memoryAllocated;
    eosSize m_memoryFreed;
    FILE*   m_file;
    eosBool m_opened;
};



EOS_NAMESPACE_END