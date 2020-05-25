#pragma once

#include "Core/BasicTypes.h"

// default alignment
#ifdef EOS_x64
#define EOS_MEMORY_ALIGNMENT_SIZE	16
#else
#define EOS_MEMORY_ALIGNMENT_SIZE	8
#endif

// Memory alignment
#define EOS_MEMORY_ALIGN(x)	__declspec(align(x))
#define EOS_ALIGN(x)			EOS_MEMORY_ALIGN(x)

#define EOS_ALIGN_PLATFORM		EOS_ALIGN(EOS_MEMORY_ALIGNMENT_SIZE)

#define EOS_ALIGN_8				EOS_ALIGN(8)
#define EOS_ALIGN_16			EOS_ALIGN(16)

#define EOS_IS_ALIGNED(ptr, alignment)    (((eos::uintPtr)ptr & (alignment - 1)) == 0)
