#pragma once

#include <chrono>
#include <string>
#include <sstream>

#include "Core/BasicTypes.h"


EOS_NAMESPACE_BEGIN


#if defined(NDEBUG)

struct LogSourceInfo
{
	LogSourceInfo(const char* _fileName, uint32 _lineNumber)
	{
	}
};


class MemoryLog
{
public:
	MemoryLog(const char*) {}
	~MemoryLog() {}
	EOS_INLINE void OnAllocation(void*, size, size, const LogSourceInfo&) const {}
	EOS_INLINE void OnDeallocation(void*, size) const {}
	EOS_INLINE size GetNumAllocations() const { return 0; }
	EOS_INLINE size GetAllocatedSize() const { return 0; }
	EOS_INLINE void Flush(size _allocated, size _used, size _total) {}
	EOS_INLINE void Reset() {}
};

#define EOS_ALLOCATION_INFO eos::LogSourceInfo(__FILE__, __LINE__)

#else

struct LogSourceInfo
{
	LogSourceInfo(const char* _fileName, uint32 _lineNumber) : m_fileName(_fileName), m_lineNumber(_lineNumber)
	{
	}

	const char* m_fileName;
	uint32 m_lineNumber;
};

#define EOS_ALLOCATION_INFO eos::LogSourceInfo(__FILE__, __LINE__)


class MemoryLog
{
public:
	MemoryLog(const char* _name)
	{
		if (!m_opened)
		{
			if (fopen_s(&m_file, (std::string(_name) + ".csv").c_str(), "w") == 0)
			{
				m_opened = true;

				m_start = std::chrono::high_resolution_clock::now();

				std::ostringstream os;
				os << "Time(ms),Type,Pointer,Size,Alignment,File,Line" << std::endl;

				fputs(os.str().c_str(), m_file);

				m_opened = true;
			}
		}
	}

	~MemoryLog()
	{
		if (m_opened)
		{
			if (m_file != nullptr)
			{
				fclose(m_file);
				m_file = nullptr;
			}

			m_opened = false;
		}
	}

	void Flush(size _allocated, size _used, size _total)
	{
		if (m_opened)
		{
			size numOfAllocations = GetNumAllocations();
			size wasted = _total - _used;

			std::ostringstream os;
			os <<
				std::endl << "Number allocation left,Memory allocation left,Memory used,Memory wasted" << std::endl <<
				numOfAllocations << "," << _allocated << "," << _used << "," << wasted << std::endl;

			fputs(os.str().c_str(), m_file);
		}
	}

	EOS_INLINE void OnAllocation(void* _ptr, size _size, size _alignment, const LogSourceInfo& _info)
	{
		++m_numAllocations;
		m_usedSize += _size;

		if (m_opened)
		{
			auto end = std::chrono::high_resolution_clock::now();
			auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - m_start).count();

			std::ostringstream os;
			os << ms << "," << "Allocation,0x" << _ptr << "," << _size << "," << _alignment << "," << _info.m_fileName << "," << _info.m_lineNumber << std::endl;

			fputs(os.str().c_str(), m_file);
		}
	}

	EOS_INLINE void OnDeallocation(void* _ptr, size _size)
	{
		--m_numAllocations;
		m_usedSize -= _size;

		if (m_opened)
		{
			auto end = std::chrono::high_resolution_clock::now();
			auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - m_start).count();

			std::ostringstream os;
			os << ms << "," << "Deallocation,0x" << _ptr << "," << _size << std::endl;

			fputs(os.str().c_str(), m_file);
		}
	}

	EOS_INLINE size GetNumAllocations() const
	{
		return m_numAllocations;
	}

	EOS_INLINE size GetAllocatedSize() const
	{
		return m_usedSize;
	}

	EOS_INLINE void Reset()
	{
		m_numAllocations = 0;
		m_usedSize = 0;
	}

private:
	std::chrono::high_resolution_clock::time_point m_start;
	size m_numAllocations = 0;
	size m_usedSize = 0;

	FILE* m_file = nullptr;
	bool m_opened = false;
};

#endif


EOS_NAMESPACE_END