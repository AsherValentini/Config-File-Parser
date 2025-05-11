#Configuration File Parser 
#  Configuration File Parser with Custom Memory Reuse Pool Allocator

This C++ project implements a lightweight configuration file parser backed by a `std::map` using a **custom STL-compatible memory reuse pool allocator**.

---

##  Features

-  Parses simple `key=value` formatted config files
-  Ignores comments and whitespace
-  Stores entries in a `std::map<std::string, std::string>`
-  Replaces default heap allocator with a **custom memory pool**
-  Logs all allocations and deallocations for tracing

---

## Why a Custom Allocator?

`std::map` internally allocates a heap node for every inserted key-value pair. In performance-critical or embedded systems, this is:
- Too slow
- Memory-fragmentation prone
- Unpredictable

This project injects a `ReusePoolAllocator` that pre-allocates memory and recycles it, offering:
- Faster allocation
- Predictable memory use
- Full control over memory behavior




