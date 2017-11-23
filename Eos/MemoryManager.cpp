#include "MemoryManager.h"

EOS_NAMESPACE_BEGIN

MemoryManager& MemoryManager::Instance()
{
    static MemoryManager instance;
    return instance;
}

EOS_NAMESPACE_END
