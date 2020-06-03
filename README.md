# Eos

Eos is a memory allocator providing a lot of pattern to help with the game development.

> Eos is a ancient greek word means Dawn.


## Important about this last rework

I made 2 major rework of this allocator, for the following reasons:
1. I started most to learn, than I noticed that my original allocator was slow
2. Afterward I approached a extensive meta-template programming paradigm and I speed that up a lot, the problem is that I copied many part of code around the web, to learn, but doing in this way it losts what I made by my own
3. This rework was in order to take control back, so it simpler than before but it is made by my experience.


## Specification

Eos is providing different allocators to use in different context in your game, and of course can be extended.

1. Linear Allocator
   - Is the fastest allocator in Eos
   - It starts always from the end of the buffer
   - The memory can't be free

2. Pool Allocator
	- Pre allocate chunk of memory of fixed size
	- just get and return the chunk during the allocation/deallocation

3. FreeList Allocator
	- Is the most versatile
	- Can be First fit or Best fit

> All these allocators does not allocate or deallocate memory, but just use some mechanism (Linear/Pool/FreeList) to manage chunk of pre allocated memory.


## Extending the allocators

Is possible to add new allocators just following the simple implementation below:

```cpp
class MyNewAllocator
{
public:
	static constexpr bool kAllowedAllocationArray = true;

	MyNewAllocator(void* _start, void* _end, size _headerSize, size _footerSize) {}
	~MyNewAllocator() {}
	EOS_INLINE void* Allocate(size _size, size _alignment, size _headerSize, size _footerSize) {}
	EOS_INLINE void Free(void* _ptr, size _size) {}
	EOS_INLINE size GetAllocatedSize(void* _ptr);
	EOS_INLINE void Reset() {}
	EOS_INLINE size GetUsedMemory()  const {}
	EOS_INLINE size GetTotalMemory() const {}
}
```

Note, the above example IS NOT AN INTERFACE, is a PROXY!
**Eos** is using extensive meta-template programming paradigm, and so your allocator just have that functions inside.
Also the parameters are not mandatory: you can have all or just commenting out whatever you do not need
(See code inside the Allocators foldr to understand the way)

Another special mechanism is the *constexpr kAllowedAllocationArray*, you can set to true or to false, and what it does is simple:
It decide if you can allocate an array of memory or not.
Now, why? Well, in the case of the Pool Allocator for instance, you already have an array of elements, and you want to use directly, so setting this to false it prevent the underneath mechanism
to split the chunk of memory when an array is requested, but simple pass the request directly to the allocator.

The actual allocation and deallocation is made from the MemoryArea (see MemoryAreaPolicy.h) and you can have stack based (meaning allocated in the stack) or heap based (heap)

Note for Realloc:
Reallocation is happening using allocation and free. The only constraint is implementing the GetAllocatedSize function which will return the size stored in the allocator.
Remember that it needs to take into account all the calculation made from the MemoryAllocator!

## Parts of the allocator

When define the allocator for your use, you have to "compose" via template, so the parts are:

1. AllocationPolicy
	- Is the actual allocator policy to use, it is a template by itself, which is made up from your allocator and the header policy
	- You can add your own allocator
	- You can create also different type of header policy as well
	- For instance: `AllocationPolicy<LinearAllocator, AllocationHeader>;`

2. Thread policy
	- Can be multi thread or single thread
	- You can create different Thread policy if you need
		- `MultiThreadPolicy`
		- `SingleThreadPolicy`
		
3. Bounds Check
	- Is used to check the memory boundaries in debug (usually)
	- You can create different bound check policy if you need
		- `MemoryBoundsCheck`

4. Memory tagging
	- Used to tag the memory, so can be used later on custom profiler/analizer
	- You can create different memory tagging policy if you need
		- `MemoryTag`

5. Memory Log
	- Simple logger which keep track of allocations/deallocations and flush he data on CSV file at the end
	- You can create different logger
		- `MemoryLog`


## Define allocator

When you have what do you need, the simplest way is define a combination and use "using" or a macro or a typedef in order to have a simple name to use
For instance: `using MyFreeListAllocator = MemoryAllocator<FreeListBestSearchAllocationPolicy, MultiThreadPolicy, MemoryBoundsCheck, MemoryTag, MemoryLog>;`
So later yu can refer to your allocator only by the "shortname"


## Use of an Allocator define

After you have defined an allocator, as explained above, you can use it.
To use it you have to pass from the memory function (or you can call directly, but is not raccommended)
The functions, macro, provided are:

- `eosNewAlignedRaw(Size, Allocator, Alignment)`
- `eosDeleteRaw(Ptr, Allocator)`
- `eosNewAligned(Type, Allocator, Alignment, ...)`
- `eosNew(Type, Allocator, ...)`
- `eosDelete(Object, Allocator)`
- `eosReallocAligned(Ptr, Type, Allocator, Alignment)`
- `eosReallocAlignedRaw(Ptr, Size, Allocator, Alignment)`
- `eosNewDynamicArray(Type, Count, Allocator)`
- `eosNewArray(Type, Allocator)`
- `eosDeleteArray(ObjectArray, Allocator)`


For instance you can use as follow:

```cpp
class Test
{
public:
	Test()
	{
		m_i = 1;
	}

	~Test()
	{
		m_i = 0;
	}

private:
	int m_i;
};

void main()
{
	HeapArea<256> simpleHeapArea;
	MemoryAllocator<LinearAllocationPolicy, SingleThreadPolicy, MemoryBoundsCheck, MemoryTag, MemoryLog> testLinearAllocator(simpleHeapArea, "Test_LinearAllocator");

	Test* t1 = eosNew(Test, &testLinearAllocator);
	eosDelete(t1, &testLinearAllocator);
}
```


## Smart pointers

Eos implememnts smart pointers.
You have to inherit from SmartObject your class you want to be a smart pointer.
Afterwards you allocate as explained above, using eosNew or any other type of macro.
When it is allocated, you "cast" it into a smart pointer.
When the smart pointer is out of scope, the internal reference counter is decremented and when it reaches 0, it will call the eosDelete

For instance you can use as follow:

```cpp
class SmartTest : public SmartObject
{
public:
	Test()
	{
		m_i = 1;
	}

	~Test()
	{
		m_i = 0;
	}

private:
	int m_i;
};

using MyLinearAllocator = MemoryAllocator<LinearAllocationPolicy, SingleThreadPolicy, MemoryBoundsCheck, MemoryTag, MemoryLog>;
void main()
{
	HeapArea<256> simpleHeapArea;
	MyLinearAllocator testLinearAllocator(simpleHeapArea, "Test_LinearAllocator");

	SmartTest* t1 = eosNew(SmartTest, &testLinearAllocator);

	// scoped to show the delete after ref counter reach 0
	{
		SmartPointer<SmartTest, MyLinearAllocator> smartT1 = SmartPointer<SmartTest, MyLinearAllocator>(&testLinearAllocator, t1);
	}
}
```

## STL

Eos implements custom containers for STL and have a bunch of wrapped stl functions ready-to-use.
So you can use the vector, string, etc... provided by Eos and adding new containers when you need more.
The STL containers have special template signature, a function call, which is used when the container is created to allocate the memory.
In this way each containers can have custom allocators and they can be allocated also on the stack with default constructor.
As forth template parameter, you can also specify a custom alignment, by default is algned to the T parameter passed.

For instance:

```cpp
MemoryAllocator<FreeListBestSearchAllocationPolicy, SingleThreadPolicy, MemoryBoundsCheck, MemoryTag, MemoryLog>* GetFreeListAllocator()
{
	static HeapArea<4096> freeListHeapArea;
	static MemoryAllocator<FreeListBestSearchAllocationPolicy, SingleThreadPolicy, MemoryBoundsCheck, MemoryTag, MemoryLog> testFreeListBestAllocator(freeListHeapArea, "Test_FreeListBestAllocator");

	return &testFreeListBestAllocator;
}

class Test
{
public:
	Test()
	{
		m_i = 1;
	}

	~Test()
	{
		m_i = 0;
	}

private:
	int m_i;
};

void main()
{
	// same signature above
	using FreeListAllocator = MemoryAllocator<FreeListBestSearchAllocationPolicy, SingleThreadPolicy, MemoryBoundsCheck, MemoryTag, MemoryLog>;

	Vector<Test, FreeListAllocator, GetAllocator>& testVector;
}
```

## Example

There is a file Test.cpp with some example.
You can also check my Vulkan Engine ION (https://github.com/KabalMcBlade/Ion) which is using this allocator



# Build Status

| Platform | Build Status |
|:--------:|:------------:|
| Windows (Visual Studio 2017) | [![Windows Build Status](https://ci.appveyor.com/api/projects/status/github/kabalmcblade/eos?branch=master&svg=true)](https://ci.appveyor.com/project/kabalmcblade/eos) |

