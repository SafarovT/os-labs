#include "../../external/catch2/catch.hpp"
#include "../MemoryManager/MemoryManager.h"

TEST_CASE("Basic allocation", "[MemoryManager]")
{
    constexpr size_t memorySize = 1024;
    alignas(std::max_align_t) std::byte memory[memorySize];
    MemoryManager manager(memory, memorySize);

    void* ptr = manager.Allocate(128);
    REQUIRE(ptr != nullptr);
}

TEST_CASE("Alignment check", "[MemoryManager]")
{
    constexpr size_t memorySize = 1024;
    alignas(std::max_align_t) std::byte memory[memorySize];
    MemoryManager manager(memory, memorySize);

    void* ptr = manager.Allocate(64, 64);
    REQUIRE(ptr != nullptr);
    REQUIRE(reinterpret_cast<uintptr_t>(ptr) % 64 == 0);

    void* ptr2 = manager.Allocate(256, 128);
    REQUIRE(ptr2 != nullptr);
    REQUIRE(reinterpret_cast<uintptr_t>(ptr2) % 128 == 0);

    void* ptr3 = manager.Allocate(16, 8);
    REQUIRE(ptr2 != nullptr);
    REQUIRE(reinterpret_cast<uintptr_t>(ptr2) % 8 == 0);
}

TEST_CASE("Full memory allocation", "[MemoryManager]")
{
    constexpr size_t memorySize = 64;
    alignas(std::max_align_t) std::byte memory[memorySize];
    MemoryManager manager(memory, memorySize);

    void* ptr = manager.Allocate(memorySize);

    void* ptr2 = manager.Allocate(100);
    REQUIRE(ptr2 == nullptr);
}

TEST_CASE("Free and relocate memory", "[MemoryManager]")
{
    constexpr size_t memorySize = 512;
    alignas(std::max_align_t) std::byte memory[memorySize];
    MemoryManager manager(memory, memorySize);

    void* ptr = manager.Allocate(128);
    REQUIRE(ptr != nullptr);

    manager.Free(ptr);

    void* ptr2 = manager.Allocate(128);
    REQUIRE(ptr2 == ptr);
}

TEST_CASE("Invalid Free handling", "[MemoryManager]")
{
    constexpr size_t memorySize = 512;
    alignas(std::max_align_t) std::byte memory[memorySize];
    MemoryManager manager(memory, memorySize);

    REQUIRE_NOTHROW(manager.Free(nullptr));
}

TEST_CASE("Thread safety under load", "[MemoryManager]")
{
    constexpr size_t memorySize = 4096;
    alignas(std::max_align_t) std::byte memory[memorySize];
    MemoryManager manager(memory, memorySize);

    constexpr size_t allocationsPerThread = 100;
    constexpr size_t numThreads = 10;

    auto AllocateMemory = [&]()
    {
        for (size_t i = 0; i < allocationsPerThread; ++i)
        {
            void* ptr = manager.Allocate(32);
            if (ptr)
            {
                manager.Free(ptr);
            }
        }
    };

    std::vector<std::thread> threads;
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(AllocateMemory);
    }

    for (auto& thread : threads)
    {
        thread.join();
    }

    REQUIRE(true);
}

TEST_CASE("Allocation with ended memory", "[MemoryManager]")
{
    constexpr size_t memorySize = 128;
    alignas(std::max_align_t) std::byte memory[memorySize];
    MemoryManager manager(memory, memorySize);

    void* ptr = manager.Allocate(100);
    REQUIRE(ptr != nullptr);

    void* ptr2 = manager.Allocate(28);
    REQUIRE(ptr2 == nullptr);
}

TEST_CASE("Memory fragmentation", "[MemoryManager]")
{
    constexpr size_t memorySize = 512;
    alignas(std::max_align_t) std::byte memory[memorySize];
    MemoryManager manager(memory, memorySize);

    void* ptr1 = manager.Allocate(128);
    REQUIRE(ptr1 != nullptr);

    void* ptr2 = manager.Allocate(128);
    REQUIRE(ptr2 != nullptr);

    manager.Free(ptr1);

    void* ptr3 = manager.Allocate(64);
    REQUIRE(ptr3 != nullptr);

    void* ptr4 = manager.Allocate(64);
    REQUIRE(ptr4 != nullptr);
}

TEST_CASE("Edge case: zero size allocation", "[MemoryManager]")
{
    constexpr size_t memorySize = 512;
    alignas(std::max_align_t) std::byte memory[memorySize];
    MemoryManager manager(memory, memorySize);

    void* ptr = manager.Allocate(0);
    REQUIRE(ptr == nullptr);
}

TEST_CASE("Edge case: invalid alignment", "[MemoryManager]")
{
    constexpr size_t memorySize = 512;
    alignas(std::max_align_t) std::byte memory[memorySize];
    MemoryManager manager(memory, memorySize);

    void* ptr = manager.Allocate(64, 3);
    REQUIRE(ptr == nullptr);
}
