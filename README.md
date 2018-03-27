# Eos

Eos is a memory allocator based on the idea that is faster allocate a large amount of memory at the begin of the application and than displacing, instead to allocate memory when need it.

> Eos is a ancient greek word means Dawn.

 
It comes with 3 different type of allocators:
1. Heap allocator
2. Stack allocator
3. Linear allocator


The difference between these allocators is in the way they 'allocate' and 'deallocate' the memory

> The real allocation and deallocation of memory is **ONLY** at the begin and at the end of the program.
> In the lifecycle of the application, the word 'allocate' means  get a portion of the memory and 'deallocate' means mark the same portion of memory as free and reusable, if requested


### Heap allocator

This is the most complex allocator implemented so far and it would be the most used.
- It is fast enough for the 'allocation' process
	- Keep track of the free chunks (need to iterate from the begin)
	- It reuses the free chunk if new request is small enough, otherwise it'll use a new chunk at the end of buffer
- Can release chunk to reuse it
- Can reallocate memory


### Stack allocator

- Use a block count per block size logic
	- Keep track of the free blocks (need to iterate from the begin)
	- Reuse the free block if new request fit it
- Can release block to reuse it
- Can reallocate memory


### Linear allocator

- Fast as possible
	- Start always from the end of the buffer
- Cannot release memory
- Cannot reallocate memory


## STL Containers

Eos implements custom containers for STL and have a bunch of wrapped stl functions ready-to-use.
So you can use the vector, string, etc... provided by Eos and adding new containers when you need more.


## Smart pointers

Eos implememnts smart pointers.
It implements 2 kind of smart pointers:
- Smart pointers
	- You've to allocate your object and than set to a smart pointer handler
- Auto Smart pointers
	- Automatically create inside itself the object at declaration time


## Memory Manager Singleton

The memory manager is a singleton class which wrap the allocators. 
You **MUST** initialize at the begin of your program and yoy **MUST** shutdown at the end of the program.


```cpp
    MemoryManager::Instance().GetHeapAllocator().Init(ALL_HEAP_MEMORY);
    MemoryManager::Instance().GetLinearAllocator().Init(ALL_LINEAR_MEMORY);
    MemoryManager::Instance().GetStackAllocator().Init(ALL_STACK_MEMORY, MAX_STACK_MEMORY_BLOCK);
```

and

```cpp
    MemoryManager::Instance().GetHeapAllocator().Shutdown();
    MemoryManager::Instance().GetLinearAllocator().Shutdown();
    MemoryManager::Instance().GetStackAllocator().Shutdown();
```


## New and Delete

You can use the wrapper for new and delete functions provided by Eos, or you can simply direct call the Memory Manager or even call the allocators manually.
Anyway my advise is to use the code/logic provided by Eos.


### Some code explanation

1. Define own allocators aligment
	- Also the memory allocators have it own aligment, the costant where it is defined is `EOS_MEMORY_ALIGNMENT_SIZE`, default is 16 but you can change according with the aligment memory rules.

2. Smart pointers
	- If you decide to use the smart pointer, remember to inherit from `SmartObject`

3. New and Delete
	- Eos implements the define for new and delete functions, these are `eosNew` and `eosDelete`, for both of theme there are different versions:
		- The `eosNew` and `eosDelete` are used for Heap Allocator
		- The `eosNewLinear` and `eosDeleteLinear` are used for Linear Allocator
		- The `eosNewStack` and `eosDeleteStack` are used for Stack Allocator
		- Moreover implements the same above but for "plain" memory, they are called in the same way adding a "raw" at the end of the definition, like `eosNewRaw` and `eosDeleteRaw` or  `eosNewRawLinear` and `eosDeleteRawLinear`

4. Trace memory
	- Defining EOS_MEMORYLOAD (in CoreDef.h now) enable you to trace the memory used, you need also to be in debug (or define _DEBUG)
	- It will create 3 files:
		- heap.log, having the tracking of the memory for the heap allocator
		- linear.log, having the tracking of the memory for the linear allocator
		- stack.log, having the tracking of the memory for the stack allocator
	- Moreover at the begin of the file you will fine the total memory allocated to the allocators by you and at the end the total memory **ACTUALLY** used by your application, this help you to tweaking the total memory to allocate the next time
	- Help also to track the pointers and if at the end of the application will be any memory leak.


## Example

In the file Test.cpp in the root folder you can find some examples how to use it.
The source code of Eos is inside the subfolder Eos


# Build Status

| Platform | Build Status |
|:--------:|:------------:|
| Windows (Visual Studio 2015) | [![Windows Build Status](https://ci.appveyor.com/api/projects/status/github/kabal2013/eos?branch=master&svg=true)](https://ci.appveyor.com/project/Kabal2013/eos) |


# License

```
Copyright 2017 Michele Condo'

                    GNU GENERAL PUBLIC LICENSE
                       Version 3, 29 June 2007

 Copyright (C) 2007 Free Software Foundation, Inc. <http://fsf.org/>
 Everyone is permitted to copy and distribute verbatim copies
 of this license document, but changing it is not allowed.

                            Preamble

  The GNU General Public License is a free, copyleft license for
software and other kinds of works.
```
