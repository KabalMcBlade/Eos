#pragma once



// comment to avoid memory trace
// Anyway this work ONLY in debug due the _DEBUG defined in the project options
#define EOS_MEMORYLOAD

#define EOS_MEMORY_ALIGNMENT_SIZE 16

// when EOS_HEAP_MEMORY_USE_BUDDY is defined (like by default) the system is using the buddy logic, this work ONLY WITH THE HEAP ALLOCATOR, other allocators do not need it.
// It is freeing every "next" unused memory blocks after the one which "delete" was requested and merge them all together, 
// otherwise it just free the one requested and the system rely on the fact to find later another allocation which will fill this free chunk with the same very size or less
// The buddy logic help to find more empty space for new allocations, but it is slower then "Not buddy logic allocator"
#define EOS_HEAP_MEMORY_USE_BUDDY   

// PLEASE UPDATE IN CASE YOU NEED IT!
#define EOS_MEMORY_1_MB             1048576ULL
#define EOS_MEMORY_4_MB             EOS_MEMORY_1_MB * 4ULL
#define EOS_MEMORY_8_MB             EOS_MEMORY_4_MB * 2ULL
#define EOS_MEMORY_16_MB            EOS_MEMORY_8_MB * 2ULL
#define EOS_MEMORY_32_MB            EOS_MEMORY_16_MB * 2ULL
#define EOS_MEMORY_64_MB            EOS_MEMORY_32_MB * 2ULL
#define EOS_MEMORY_128_MB           EOS_MEMORY_64_MB * 2ULL
#define EOS_MEMORY_256_MB           EOS_MEMORY_128_MB * 2ULL
#define EOS_MEMORY_512_MB           EOS_MEMORY_256_MB * 2ULL
#define EOS_MEMORY_1024_MB          EOS_MEMORY_512_MB * 2ULL
#define EOS_MEMORY_2048_MB          EOS_MEMORY_1024_MB * 2ULL
#define EOS_MEMORY_4096_MB          EOS_MEMORY_2048_MB * 2ULL
#define EOS_MEMORY_8192_MB          EOS_MEMORY_4096_MB * 2ULL


// PLEASE CHANGE IN ORDER TO MATCH YOUR REQUIREMENTS!

#define EOS_STL_MAX_HEAP_MEMORY         EOS_MEMORY_1024_MB
#define EOS_STL_MAX_STACK_MEMORY_SIZE   EOS_MEMORY_1_MB
#define EOS_STL_MAX_LINEAR_MEMORY       EOS_MEMORY_1_MB

#define EOS_HEAP_MEMORY             EOS_STL_MAX_HEAP_MEMORY + (EOS_MEMORY_1024_MB)
#define EOS_LINEAR_MEMORY           EOS_STL_MAX_LINEAR_MEMORY
#define EOS_STACK_BLOCK_SIZE        EOS_STL_MAX_STACK_MEMORY_SIZE
#define EOS_STACK_BLOCK_COUNT       2