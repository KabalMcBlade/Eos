#include <SDKDDKVer.h>

#include <stdio.h>
#include <tchar.h>

#include "Eos/Eos.h"


#define MEMORY_1_MB                 1048576
#define MEMORY_4_MB                 MEMORY_1_MB * 4
#define MEMORY_8_MB                 MEMORY_4_MB * 2
#define MEMORY_16_MB                MEMORY_8_MB * 2

#define STL_MAX_HEAP_MEMORY         MEMORY_16_MB
#define STL_MAX_STACK_MEMORY_SIZE   MEMORY_4_MB
#define STL_MAX_LINEAR_MEMORY       MEMORY_8_MB


#define MAX_STACK_MEMORY_BLOCK      1024
#define ALL_HEAP_MEMORY             STL_MAX_HEAP_MEMORY + (MEMORY_16_MB)
#define ALL_LINEAR_MEMORY           STL_MAX_LINEAR_MEMORY + (MEMORY_16_MB * 2)
#define ALL_STACK_MEMORY            STL_MAX_STACK_MEMORY_SIZE


EOS_USING_NAMESPACE

EOS_OPTIMIZATION_OFF

class Cat
{
public:
    int a;

    Cat()
    {
        a = 0xABCD;
    }

    Cat(const Cat& other)
    {
        a = other.a;
    }

    Cat& operator=(const Cat& other)
    {
        a = other.a;
        return *this;
    }

    Cat(int _a)
    {
        a = _a;
    }

    virtual ~Cat()
    {
        a = 0x0000;
    }
};

class SmartCat : public SmartObject
{
public:
    int a;

    SmartCat()
    {
        a = 0xCAFE;
    }

    SmartCat(const SmartCat& other)
    {
        a = other.a;
    }

    SmartCat& operator=(const SmartCat& other)
    {
        a = other.a;
        return *this;
    }

    SmartCat(int _a)
    {
        a = _a;
    }

    virtual ~SmartCat()
    {
        a = 0x0000;
    }
};

class SuperCat : public Cat
{
public:
    char counter[10];
    int numlol;

    SuperCat() : Cat()
    {
        memcpy(counter, "xxxxxxxxxx", 10);
        numlol = 101;
    }

    SuperCat(int _a) : Cat(_a)
    {
        memcpy(counter, "0123456789", 10);
    }

    virtual ~SuperCat()
    {
        numlol = 0;
        memset(counter, 0, 10);
    }
};


int main()
{
    MemoryManager::Instance().GetHeapAllocator().Init(ALL_HEAP_MEMORY);
    MemoryManager::Instance().GetLinearAllocator().Init(ALL_LINEAR_MEMORY);
    MemoryManager::Instance().GetStackAllocator().Init(ALL_STACK_MEMORY, MAX_STACK_MEMORY_BLOCK);
    
    // 1. Using manual allocation:
    {
        Cat* catPtr = eosNew(Cat, EOS_MEMORY_ALIGNMENT_SIZE);
        eosDelete(catPtr);

        Cat* catPtr2 = eosNew(Cat, EOS_MEMORY_ALIGNMENT_SIZE, 43);
        eosDelete(catPtr2);
    }

    // 2. Using smart pointer with manual allocation and automatically deallocation
    {
        SmartCat* catPtr = eosNew(SmartCat, EOS_MEMORY_ALIGNMENT_SIZE);
        SmartPointer<SmartCat> catSmart(catPtr);

        SmartPointer<SmartCat> catSmartClone1 = catSmart;
        SmartPointer<SmartCat> catSmartClone2 = catSmart;
        SmartPointer<SmartCat> catSmartClone3 = catSmart;
    }

    // 3. Using smart pointer with automatically allocation and automatically deallocation
    {
        AutoSmartPointer<SmartCat, EOS_MEMORY_ALIGNMENT_SIZE> catPtr;

        AutoSmartPointer<SmartCat, EOS_MEMORY_ALIGNMENT_SIZE, int> catPtrVariadic(10);
    }
    
    // 4. Realloc Test
    // This one have no sense I know, but I did it, so enjoy xD
    // NOTE: exit the eosNewRaw for "plain" memory, so if you using that you can also use the eosReallocRaw, that actually works as expected :D
    {
        Cat* catPtr = eosNew(Cat, EOS_MEMORY_ALIGNMENT_SIZE);
        Cat* catPtr2 = eosRealloc(catPtr, Cat, sizeof(Cat) * 2, EOS_MEMORY_ALIGNMENT_SIZE);

        eosDelete(catPtr2);
    }
    
    // 5. Superclass Test
    {
        SuperCat* superCatPtr = eosNew(SuperCat, EOS_MEMORY_ALIGNMENT_SIZE);
        eosDelete(superCatPtr);

        SuperCat* superCatPtr2 = eosNew(SuperCat, EOS_MEMORY_ALIGNMENT_SIZE, 10);
        eosDelete(superCatPtr2);
    }
    
    // 6. Containers Test
    {
        eosVector(Cat) catArray;
        catArray.resize(10);
        catArray.clear();
        
        eosVector(SmartCat) smartCatArray;
        smartCatArray.resize(10);
        smartCatArray.clear();
    }
    

    MemoryManager::Instance().GetHeapAllocator().Shutdown();
    MemoryManager::Instance().GetLinearAllocator().Shutdown();
    MemoryManager::Instance().GetStackAllocator().Shutdown();


    return 0;
}

