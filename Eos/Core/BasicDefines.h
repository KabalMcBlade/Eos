// Copyright (c) 2018-2025 Michele Condo'
// File: C:\Projects\Eos\Eos\Core\BasicDefines.h
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once


#ifdef EOS_EXPORTS
#define EOS_DLL __declspec(dllexport)
#else
#define EOS_DLL __declspec(dllimport)
#endif 


#if _WIN32 || _WIN64
#if _WIN64
#define EOS_x64
#else
#define EOS_x86
#endif
#elif __GNUC__
#if __x86_64__ || __ppc64__
#define EOS_x64
#else
#define EOS_x86
#endif
#else
#define EOS_x86
#endif


// defines for easy namespace
#define EOS_NAMESPACE_BEGIN namespace eos {
#define EOS_NAMESPACE_END };  

#define EOS_USING_NAMESPACE using namespace eos; 

#define EOS_OPTIMIZATION_OFF __pragma(optimize("",off))
#define EOS_OPTIMIZATION_ON __pragma(optimize("",on))

/// forces a function to be in lined
#define EOS_INLINE __forceinline

// tells the compiler to never inline a particular function
#define EOS_NO_INLINE  __declspec(noinline)

