# High-Performance Fixed-Size Memory Pool Allocator (C++)

## 📌 Overview
In ultra-low-latency environments like High-Frequency Trading (HFT), dynamic memory allocation (`new`/`malloc`) is a critical bottleneck. Asking the Operating System for memory causes context switches, unpredictable latency spikes, and heap fragmentation.

This project implements a **Zero-Overhead, Fixed-Size Memory Pool Allocator** in Modern C++. It pre-allocates a single contiguous block of heap memory at startup and manages it using an embedded free-list, reducing allocation and deallocation times to deterministic **$O(1)$ constant time**.

## 🚀 Key Features
* **Zero OS Overhead on Hot Paths:** Completely bypasses `new`/`delete` and `malloc`/`free` during runtime execution.
* **Embedded Free-List Architecture:** Unallocated memory blocks natively store the `Node*` pointing to the next available block via `reinterpret_cast`. This means tracking available memory requires **zero extra memory footprint**.
* **$O(1)$ Time Complexity:** Both `allocate()` and `deallocate()` execute in a few CPU cycles.
* **Placement New Integration:** Designed to work flawlessly with Placement New for custom object construction at specific raw memory addresses.
* **RAII Memory Safety:** The underlying massive memory chunk is wrapped in a `std::unique_ptr<char[]>`, ensuring strict cleanup and zero memory leaks upon system termination.

## 🧠 How It Works (Memory Layout)
At initialization, the pool allocates an array of bytes. Instead of keeping a separate `std::vector` to track which blocks are free, the pool casts the raw bytes of the *unallocated* blocks into a linked list:

```text
[ Free Block 1 ] -> [ Free Block 2 ] -> [ Free Block 3 ] -> nullptr
When allocate() is called, the pool detaches the head of the free-list and hands the raw void* address to the application. When deallocate(void* ptr) is called, the pool casts that address back into a Node* and pushes it back onto the front of the free-list.

💻 Usage Example
C++
#include "Fixed_size_MemoryPool.h"
#include <iostream>

struct Player {
    int id;
    int health;
};

int main() {
    // 1. Initialize a pool for 5 Player objects
    Fixed_size_MemoryPool pool(5, sizeof(Player));

    // 2. Allocate memory in O(1) time without OS intervention
    void* raw_memory = pool.allocate();
    
    // 3. Construct the object directly into the pre-allocated raw memory
    Player* p1 = new (raw_memory) Player{101, 100};

    std::cout << "Player ID: " << p1->id << " created at " << p1 << "\n";

    // 4. Manually trigger the destructor 
    p1->~Player();

    // 5. Return the raw memory back to the pool's free-list in O(1) time
    pool.deallocate(raw_memory);

    return 0;
}
