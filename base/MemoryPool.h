#ifndef __MEMORYPOOL__
#define __MEMORYPOOL__

#include "Mutex.h"
#include <cstdio>
#include <cstddef>
#include <stdint.h>
#include <stdarg.h>
#include <utility>
#include <functional>

#define BlockSize 4096 //4kb

struct Slot {
    Slot *next;
};

class MemoryPool {
public:
    ~MemoryPool();
    void init(int size);
    Slot *allocate();
    void deallocate(Slot *p);

private:
    int slot_size; //
    Slot *currentBlock;//当前Block内存块
    Slot *currentSlot;//当前Slot槽
    Slot *lastSlot;//指向该Block中最后一块可用的Slot
    Slot *freeSlot;//

    MutexLock m_freeSlot;
    MutexLock m_other;

    size_t padPointer(char *p, size_t align);
    Slot *allocateBlock();
    Slot *nofree_solve();
};

void init_memorypool();
void *use_memory(size_t size);
void free_memory(size_t size, void *p);
MemoryPool& get_memorypool(int id);

template<class T, class... Args>
T* newElement(Args&&... args) {
    T *p;
    if (p == reinterpret_cast<T*>(use_memory(sizeof(T)))) {  //reinterpret_cast 指针类型之间进行转化
        new(p)T(std::forward<Args>(args)...);    //placement new
    }
    return p;
}

template<class T>
void deleteElement(T *p) {
    if (p) {
        p->~T();
    }
    free_memory(sizeof(T), reinterpret_cast<void*> (p));
}

#endif