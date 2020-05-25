#include <SDKDDKVer.h>

#include <stdio.h>
#include <tchar.h>

#include "Eos/Eos.h"

EOS_USING_NAMESPACE


MemoryAllocator<FreeListBestSearchAllocationPolicy, SingleThreadPolicy, MemoryBoundsCheck, MemoryTag, MemoryLog>* GetFreeListAllocator()
{
	static HeapArea<4096> freeListHeapArea;
	static MemoryAllocator<FreeListBestSearchAllocationPolicy, SingleThreadPolicy, MemoryBoundsCheck, MemoryTag, MemoryLog> testFreeListBestAllocator(freeListHeapArea, "Test_FreeListBestAllocator");

	return &testFreeListBestAllocator;
}


static int incremental = 0;

class Test
{
public:
	Test()
	{
		m_i = incremental++;
	}

	~Test()
	{
		m_i = 0;
	}

private:
	int m_i;
};

class Cat
{
public:
	Cat()
	{
		m_i = incremental++;
		m_pawCount = 4;
		m_hasTail = true;
		m_color[0] = 0.5f;
		m_color[1] = 0.35f;
		m_color[2] = 0.13f;
	}

	~Cat()
	{
		m_i = 0;
		m_pawCount = 0;
		m_hasTail = false;
		memset(&m_color, 0, sizeof(m_color));
	}

private:
	int m_pawCount;
	int m_i;
	float m_color[3];
	bool m_hasTail;
};


class SmartCat : public Cat, public SmartObject
{

};


int main()
{
	HeapArea<256> simpleHeapArea;
	MemoryAllocator<LinearAllocationPolicy, SingleThreadPolicy, MemoryBoundsCheck, MemoryTag, MemoryLog> testLinearAllocator(simpleHeapArea, "Test_LinearAllocator");

	Test* t1 = eosNew(Test, &testLinearAllocator);
	eosDelete(t1, &testLinearAllocator);

	t1 = eosNew(Test, &testLinearAllocator);
	eosDelete(t1, &testLinearAllocator);

	t1 = eosNew(Test, &testLinearAllocator);
	eosDelete(t1, &testLinearAllocator);

	t1 = eosNew(Test, &testLinearAllocator);
	eosDelete(t1, &testLinearAllocator);

	Test* tArray = eosNewArray(Test[4], &testLinearAllocator);
	eosDeleteArray(tArray, &testLinearAllocator);

	Test* tArray2 = eosNewArray(Test[8], &testLinearAllocator);
	eosDeleteArray(tArray2, &testLinearAllocator);

	Test* tDynamicArray = eosNewDynamicArray(Test, 4, &testLinearAllocator);
	eosDeleteArray(tDynamicArray, &testLinearAllocator);


	///////////////////////////////////////////////////////////////////////

	HeapArea<512> poolHeapArea;
	MemoryAllocator<PoolAllocationPolicy<sizeof(Cat), alignof(Cat)>, SingleThreadPolicy, MemoryBoundsCheck, MemoryTag, MemoryLog> testPoolAllocator(poolHeapArea, "Test_PoolAllocator");

	Cat* mew0 = eosNew(Cat, &testPoolAllocator);
	eosDelete(mew0, &testPoolAllocator);

	Cat* mew1 = eosNew(Cat, &testPoolAllocator);
	eosDelete(mew1, &testPoolAllocator);

	// These 2 calls are to show that the pool allocator cannot use the array version, due is already allocated by the element count
	// so simply get one by one.
	// Uncomment to see the assert on console
	//Cat* catArray = eosNewArray(Cat[8], &testPoolAllocator);
	//eosDeleteArray(catArray, &testPoolAllocator);

	//Cat* catArrayD = eosNewDynamicArray(Cat, 4, &testPoolAllocator);
	//eosDeleteArray(catArrayD, &testPoolAllocator);
	//

	///////////////////////////////////////////////////////////////////////
	using FreeListAllocator = MemoryAllocator<FreeListBestSearchAllocationPolicy, SingleThreadPolicy, MemoryBoundsCheck, MemoryTag, MemoryLog>;
	///////////////////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////////////////////

	HeapArea<512> freeListHeapArea;
	FreeListAllocator testFreeListBestAllocator(freeListHeapArea, "Test_FreeListBestAllocator");

	Cat* kitty0 = eosNew(Cat, &testFreeListBestAllocator);
	eosDelete(kitty0, &testFreeListBestAllocator);

	Cat* kitty1 = eosNew(Cat, &testFreeListBestAllocator);
	eosDelete(kitty1, &testFreeListBestAllocator);

	kitty0 = eosNew(Cat, &testFreeListBestAllocator);
	kitty1 = eosNew(Cat, &testFreeListBestAllocator);
	eosDelete(kitty0, &testFreeListBestAllocator);
	eosDelete(kitty1, &testFreeListBestAllocator);

	///////////////////////////////////////////////////////////////////////

	HeapArea<512> smartFreeListHeapArea;
	FreeListAllocator smartTestFreeListBestAllocator(smartFreeListHeapArea, "Smart_Test_FreeListBestAllocator");

	// explicit
	SmartCat* smartCat = eosNew(SmartCat, &smartTestFreeListBestAllocator);
	{
		SmartPointer<SmartCat, FreeListAllocator> smartCatPtr1 = SmartPointer<SmartCat, FreeListAllocator>(&smartTestFreeListBestAllocator, smartCat);

		SmartPointer<SmartCat, FreeListAllocator> smartCatPtr2 = smartCatPtr1;

		{
			SmartPointer<SmartCat, FreeListAllocator> smartCatPtr3 = smartCatPtr2;
			SmartPointer<SmartCat, FreeListAllocator> smartCatPtr4 = smartCatPtr1;
		}
	}

	// implicit
	{
		SmartPointer<SmartCat, FreeListAllocator> autoSmartCatPtr1 = SmartPointer<SmartCat, FreeListAllocator>(&smartTestFreeListBestAllocator);

		SmartPointer<SmartCat, FreeListAllocator> autoSmartCatPtr2 = autoSmartCatPtr1;

		{
			SmartPointer<SmartCat, FreeListAllocator> autoSmartCatPtr3 = autoSmartCatPtr2;
			SmartPointer<SmartCat, FreeListAllocator> autoSmartCatPtr4 = autoSmartCatPtr1;
		}
	}

	///////////////////////////////////////////////////////////////////////


	Vector<Cat, FreeListAllocator, GetFreeListAllocator> catVector;
	catVector.resize(16);
}