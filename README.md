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
- Using the define EOS_HEAP_MEMORY_USE_BUDDY (defined by default) is enablig the Buddy logic to merge empty consecutive blocks


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


### Usage

- Open the file `MemoryDefines.h` and set the parameters you need:
	- Define or not the EOS_MEMORYLOAD macro, which let the allocator dump out the memory on file (see section later)
	- Define the default alignment size in the EOS_MEMORY_ALIGNMENT_SIZE macro, used for the memory class itself and the allocator header size (by default is 16 and I racommend that)
	- Define or not the EOS_HEAP_MEMORY_USE_BUDDY macro, which the allocator using the ***Buddy logic***, this means is slower but merge consecutive empty memory block in order to have bigger one later on
		- It works only with the Heap Allocator
	- Define the memory size you want:
		- EOS_HEAP_MEMORY
		- EOS_LINEAR_MEMORY
		- EOS_STACK_BLOCK_SIZE and EOS_STACK_BLOCK_COUNT

- Where you need to use the allocators add include file `Eos.h`

- Smart pointers
	1. Inherit your class/struct from `SmartObject`
	2. Handle it with `SmartPointer<>` 

- New and Delete
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
| Windows (Visual Studio 2017) | [![Windows Build Status](https://ci.appveyor.com/api/projects/status/github/kabalmcblade/eos?branch=master&svg=true)](https://ci.appveyor.com/project/kabalmcblade/eos) |
