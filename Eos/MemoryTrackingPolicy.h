#pragma once

#include <chrono>

#include "CoreDefs.h"

#include "MemoryUtils.h"

EOS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
class eosNoMemoryTracking
{
public:
    EOS_INLINE void Init(const char*) {}
    template<typename Allocator>
    EOS_INLINE void Shutdown(const Allocator&) {}
    EOS_INLINE void OnAllocation(void*, eosSize, eosSize, const eosSourceInfo&) const {}
    EOS_INLINE void OnDeallocation(void*, eosSize) const {}
    EOS_INLINE eosSize GetNumAllocations() const { return 0; }
    EOS_INLINE eosSize GetAllocatedSize() const { return 0; }
};

////////////////////////////////////////////////////////////////////////////////
class eosMemoryTracking
{
public:
    EOS_INLINE void Init(const char* _name)
    {
        if (!m_opened)
        {
            if (fopen_s(&m_file, (std::string(_name) + ".csv").c_str(), "w") == 0)
            {
                m_opened = true;

                std::ostringstream os;
                os << "Time(ms),Type,Pointer,Size,Alignment,Time(ms),File,Line number" << std::endl;

                fputs(os.str().c_str(), m_file);

                m_opened = true;
            }
        }
    }

    template<typename Allocator>
    EOS_INLINE void Shutdown(const Allocator& _allocator)
    {
        if (m_opened)
        {
            eosSize numOfAllocations = GetNumAllocations();
            eosSize allocated = _allocator.GetAllocatedSize();
            eosSize virtualMemorySize = _allocator.GetVirtualSize();
            eosSize physicalMemorySize = _allocator.GetPhysicalSize();
            eosSize used = _allocator.GetTotalUsedSize();
            eosSize wasted = _allocator.GetTotalUsedSize() - GetAllocatedSize();

			auto t0 = std::chrono::high_resolution_clock::now();
			auto nanosec = t0.time_since_epoch();
			auto ms = (nanosec.count() / 1000000);	// convert to millisecond the nanosecond

            std::ostringstream os;
            os << 
                std::endl << "Number allocation left,Memory allocation left,Virtual memory size,Physical memory size,Memory used,Memory wasted,Total time (ms)" << std::endl <<
                numOfAllocations << "," << allocated << "," << virtualMemorySize << "," << physicalMemorySize << "," << used << "," << wasted << "," << ms << std::endl;

            fputs(os.str().c_str(), m_file);

            if (m_file != nullptr)
            {
                fclose(m_file);
                m_file = nullptr;
            }

            m_opened = false;
        }
    }

    EOS_INLINE void OnAllocation(void* _ptr, eosSize _size, eosSize _alignment, const eosSourceInfo& _info)
    {
        ++m_numAllocations;
        m_usedSize += _size;

        if (m_opened)
        {
			auto t0 = std::chrono::high_resolution_clock::now();
			auto nanosec = t0.time_since_epoch();
			auto ms = (nanosec.count() / 1000000);	// convert to millisecond the nanosecond

            std::ostringstream os;
            os << ms << "," << "Allocation,0x" << _ptr << "," << _size << "," << _alignment << "," << _info.m_fileName << "," << _info.m_lineNumber << std::endl;

            fputs(os.str().c_str(), m_file);
        }
    }

    EOS_INLINE void OnDeallocation(void* _ptr, eosSize _size)
    {
        --m_numAllocations;
        m_usedSize -= _size;

        if (m_opened)
        {
			auto t0 = std::chrono::high_resolution_clock::now();
			auto nanosec = t0.time_since_epoch();
			auto ms = (nanosec.count() / 1000000);	// convert to millisecond the nanosecond

            std::ostringstream os;
            os << ms << "," << "Deallocation,0x" << _ptr << "," << _size << std::endl;

            fputs(os.str().c_str(), m_file);
        }
    }

    EOS_INLINE eosSize GetNumAllocations() const
    {
        return m_numAllocations;
    }

    EOS_INLINE eosSize GetAllocatedSize() const
    {
        return m_usedSize;
    }

    EOS_INLINE void Reset()
    {
        m_numAllocations = 0;
        m_usedSize = 0;
    }

private:
    eosSize m_numAllocations = 0;
    eosSize m_usedSize = 0;

    FILE*   m_file = nullptr;
    eosBool m_opened = false;
};

////////////////////////////////////////////////////////////////////////////////
#if defined(_DEBUG)
using eosDefaultTrackingPolicy = eosMemoryTracking;
#else
using eosDefaultTrackingPolicy = eosNoMemoryTracking;
#endif

EOS_NAMESPACE_END
