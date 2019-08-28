# Eos

Eos is a memory allocator providing a lot of patterns to help with the game development.

> Eos is a ancient greek word means Dawn.

```diff
- DOCUMENTATION IS WORK IN PROGRESS
```

Eos is providing different allocators to use in different context in your game:

1. Malloc allocator
   - This is the simplest one, is actually a simple malloc allocation doing at runtime
   - The pros to use this allocator instead of the current malloc is:
     - In *debug* you have the same helpers for tagging, checking and tracking the memory
	 - You can put a limit as max memory
2. Heap allocator
   - Is called heap allocator in the way that should be used as standard heap allocation
   - Is using a buddy allocation logic underneath
     - The size need to be a power of 2 with a max of 1 << 31 = ~2GB
   - Is very fast
     - But it suffers of fragmentation
3. Linear allocator
   - Is the fastest allocator in Eos
   - It starts always from the end of the buffer
   - The memory can't be free
4. Pool allocator
   - Can grow or not, depend which one you are using
   - Can't be used with eosNewArray or eosNewDynamicArray or STL wrapper
     - These 2 function allocate a huge buffer which is the total size of the count, but internally the Pool allocator can allocate only the max size of single element per object in pool.
5. Stack allocator
   - Can't be used with eosNewArray or eosNewDynamicArray or STL wrapper
     - These 2 function allocate a huge buffer which is the total size of the count, but internally the Pool allocator can allocate only the max size of single element per object in pool.
6. Stl allocator
7. Smart pointers


# Build Status

| Platform | Build Status |
|:--------:|:------------:|
| Windows (Visual Studio 2017) | [![Windows Build Status](https://ci.appveyor.com/api/projects/status/github/kabalmcblade/eos?branch=master&svg=true)](https://ci.appveyor.com/project/kabalmcblade/eos) |
