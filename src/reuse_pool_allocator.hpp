#pragma once

#include <cstddef>
#include <memory>
#include <iostream>
#include <vector>
#include <cassert>
/*============================================================================================================================================================

So here is my brief summary of the ReusePoolAllocator class and how to use it both with STL container and manuall with UDTs

ReusePoolAllocator is a custom memory pool allocator designed to minimize heap allocations by recycling fixed-size memory blocks.
Instead of calling new/delete for each object, it returns pointers to reusable memory blocks, and tracks which blocks are free.

It is fully STL-compatible and can be used to override allocation behavior in standard containers like std::map or std::vector,
dramatically reducing heap churn.

It can also be used manually with user-defined types (UDTs) via placement new semantics for fine-grained control.

==============================================================================================================================================================

STL Integration Example:

std::map<
    std::string, std::string,
    std::less<>,
    ReusePoolAllocator<std::pair<const std::string, std::string>>
> myMap;

Simply pass ReusePoolAllocator as the allocator template argument — and boom: the container reuses memory efficiently under the hood.

==============================================================================================================================================================

Manual Use Example with a UDT:

struct MyStruct {
    int arg1;
    std::string arg2;
    char arg3;
};

auto allocator = std::make_shared<ReusePoolAllocator<MyStruct>>();  // Shared so the deleter can capture and share ownership

int someNumber = 10;
std::string someString = "someString";
char someChar = 'c';

// Allocate raw memory
MyStruct* structPtr = allocator->allocate(1);

// Construct in-place (placement new)
new (structPtr) MyStruct{someNumber, someString, someChar};

// Define custom deleter that calls destructor and returns to pool
auto customDeleter = [allocator](MyStruct* ptr) {
    ptr->~MyStruct();
    allocator->deallocate(ptr, 1);
};

// Wrap in shared_ptr (manual version of make_shared with custom memory)
std::shared_ptr<MyStruct> mySharedStructPtr(structPtr, customDeleter);

// Now you can safely pass mySharedStructPtr around with full RAII

==============================================================================================================================================================*/

template <typename T, std::size_t PoolSize = 1024>
class ReusePoolAllocator{
public:
    // STL allocator requirement
    using value_type = T;

    ReusePoolAllocator() {
        pool_.reserve(PoolSize);
        std::cout << "[Allocator] Created with pool size: " << PoolSize << std::endl;
    }

    // STL allocator requirement
    template<typename U>
    ReusePoolAllocator(const ReusePoolAllocator<U, PoolSize>&) {}

    // Custom allocated that does not use the heap (new)
    T* allocate(std::size_t n) { //we can have up to PoolSize allocations
        if(n != 1) throw std::bad_alloc(); //we only support allocating one objeect at a time

        // Check if we have a free block memory that we can reuse before allocating a new one. Imagine the map had an element deleted, but we still have the memory allocated
        if(!free_list_.empty()) {
            T* ptr = free_list_.back();
            free_list_.pop_back();
            std::cout << "[Allocator] Reused block at: " << ptr << std::endl;
        }

        // If we have exceeded the bounds of the memory pool ie we have allocated more than 1024 objects of size T already, we throw a bad alloc exception just like the stl allocate would
        if(pool_.size() >= PoolSize) {
            throw std::bad_alloc();
        }
        // If we have not exceeded the bounds of the memory pool and we have no free blocks (no map elements deleted or even added yet) we can allocate a new block of memory to the pool
        pool_.emplace_back(); // add block to the pool, obviously not initialized
        void* raw = &pool_.back();
        T* ptr = reinterpret_cast<T*>(raw); // now we can point to that new blocks address in the pool_ vector
        std::cout << "[Allocator] New block at: " << ptr << std::endl;
        return ptr;  // return the address of the new block
    }

    // We do not want to deallocate the memory in the pool, because we want to reuse it later, but we need to know that the block is free, so we can use the pointer to the block. Simply add that pointer to the free list
    void deallocate(T* p, std::size_t n) {
        if (n != 1) return;
        std::cout <<"[Allocator] Freed block at: " << p << std::endl;
        free_list_.push_back(p);
    }

    template<typename U>
    struct rebind {
        using other = ReusePoolAllocator<U, PoolSize>;
    };

private:
    std::vector<std::aligned_storage_t<sizeof(T), alignof(T)>> pool_;
    std::vector<T*> free_list_;
};
