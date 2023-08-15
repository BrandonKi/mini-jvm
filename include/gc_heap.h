#ifndef MINI_JVM_GC_HEAP_H
#define MINI_JVM_GC_HEAP_H

#include "common.h"

template <typename T>
struct GCPtr {
    
};

class GCHeap {
public:
    GCHeap(u32 max_memory);
    
    template <typename T>
    GCPtr<T> alloc(T obj);
};

#endif // MINI_JVM_GC_HEAP_H