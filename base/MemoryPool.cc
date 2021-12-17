#include "MemoryPool.h"
//初始化
void MemoryPool::init(int size) {
    slot_size = size;
    currentBlock = currentSlot = lastSlot = freeSlot = nullptr;
}
//释放内存池开辟的空间 
MemoryPool::~MemoryPool() {
    Slot *curr = currentBlock;
    while (curr) {
        Slot *next = curr->next;
        free(reinterpret_cast<void *>(curr));
        curr = next;
    }
}
//计算类型字节数超出指针多少(方便内存对齐)
inline size_t MemoryPool::padPointer(char *p, size_t align) {
    uintptr_t result = reinterpret_cast<uintptr_t>(p);
    return ((align - result) % align);
}
//分配Block内存块
inline Slot *MemoryPool::allocateBlock() {
    char *newBlock = nullptr;
    while (!(newBlock = reinterpret_cast<char *>(malloc(BlockSize))));
    char *body = newBlock + sizeof(Slot);
    //为了类型安全
    size_t bodyPadding = padPointer(body, static_cast<size_t>(slot_size));
    Slot *useSlot;
    //加锁，RAII机制对锁进行封装，使线程更加安全
    {
        MutexlockGuard lock(m_other);
        reinterpret_cast<Slot *>(newBlock)->next = currentBlock;
        currentBlock = reinterpret_cast<Slot *>(newBlock);
        lastSlot = reinterpret_cast<Slot *>(newBlock + BlockSize - slot_size + 1);
        useSlot = currentSlot;
        currentSlot += (slot_size >> 3);
    }
    return useSlot;
}

inline Slot *MemoryPool::nofree_solve() {
    if (currentSlot >= lastSlot) {
        return allocateBlock();
    }
    Slot *useSlot;
    {
        MutexlockGuard lock(m_other);
        useSlot = currentSlot;
        currentSlot += (slot_size >> 3);
    }
    return useSlot;
}

inline Slot *MemoryPool::allocate() {
    if (freeSlot) {
        MutexlockGuard lock(m_freeSlot);
        if (freeSlot) {
            Slot *result = freeSlot;
            freeSlot = freeSlot->next;
            return result;
        }
    }
    return nofree_solve();
}

inline void MemoryPool::deallocate(Slot *p) {
    if (p) {
        MutexlockGuard lock(m_freeSlot);
        p->next = freeSlot;
        freeSlot = p;
    }
}

void *use_memory(size_t size) {
    if (!size) {
        return nullptr;
    }
    //块最大为512字节，超过直接malloc分配
    if (size > 512) {
        return malloc(size);
    }
    //保证在所属的块中分配
    return reinterpret_cast<void *>(get_memorypool(((size + 7) >> 3) - 1).allocate());
}

void free_memory(size_t size, void *p) {
    if (!p) {
        return;
    }
    if (size > 512) {
        free(p);
        return;
    }
    get_memorypool(((size + 7) >> 3) - 1).deallocate(reinterpret_cast<Slot *>(p));
}

void init_memorypool() {
    for (int i = 0; i < 64; ++i) {
        get_memorypool(i).init((i + 1) << 3);
    }
}

MemoryPool& get_memorypool(int id) {
    //静态，维护64个块 从下标为0开始维护一块8B大小的链表，一直到64维护一块512B大小的链表
    static MemoryPool memorypool[64];
    return memorypool[id];
}
