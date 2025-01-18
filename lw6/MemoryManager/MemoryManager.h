#pragma once
#include <cassert>
#include <memory>
#include <utility>
#include <mutex>
#include <algorithm>
#include <iostream>

const size_t MIN_ALIGN = 8;

class MemoryManager
{
public:
    MemoryManager(void* start, size_t size) noexcept
    {
        assert(reinterpret_cast<uintptr_t>(start) % alignof(std::max_align_t) == 0);

        m_freeList = reinterpret_cast<DataBlock*>(start);
        m_freeList->size = size;
        m_freeList->next = nullptr;
    }

    MemoryManager(const MemoryManager&) = delete;
    MemoryManager& operator=(const MemoryManager&) = delete;

    void* Allocate(size_t size, size_t align = alignof(std::max_align_t)) noexcept
    {
        std::scoped_lock<std::mutex> lock(m_mutex);

        if (size == 0 || (align & (align - 1)) != 0)
        {
            return nullptr;
        }

        if (align < 8)
        {
            int multiplier = (8 + align - 1) / align;
            align *= multiplier;
        }

        DataBlock** prev = &m_freeList;
        DataBlock** current = &m_freeList;

        const size_t metaSize = sizeof(DataBlock);

        while (*current)
        {
            auto addr = reinterpret_cast<uintptr_t>((*current)) + metaSize;
            auto alignedStart = (addr + align - 1) & ~(align - 1);
            auto padding = alignedStart - reinterpret_cast<std::uintptr_t>(*current);

            // сами блоки с данными align
            // адресс по которому размещается DataBlock должен быть кратен DataBlock
            // выровнять минимум по 8 (1 например кратна 8, поэтому если просят по 1, то выравниваем по 8)

            if ((*current)->size >= size + padding)
            {

                size_t totalAllocationSize = size + padding;
                if ((*current)->size > totalAllocationSize + metaSize)
                {
                    auto newBlockStart = (alignedStart + size - 1) & ~(sizeof(align) - 1);
                    DataBlock* newDataBlock = reinterpret_cast<DataBlock*>(newBlockStart);
                    newDataBlock->size = (*current)->size - padding - size;
                    newDataBlock->next = (*current)->next;
                    (*current)->size = padding;
                    (*current)->next = newDataBlock;
                }

                *prev = (*current)->next;
                DataBlock* allocatedBlock = reinterpret_cast<DataBlock*>(alignedStart - metaSize);
                allocatedBlock->size = size + metaSize;
                return reinterpret_cast<void*>(alignedStart);
            }

            current = &((*current)->next);
        }

        return nullptr;
    }

    void Free(void* addr) noexcept
    {
        if (addr == nullptr)
        {
            return;
        }

        std::scoped_lock<std::mutex> lock(m_mutex);

        auto* blockToFree = reinterpret_cast<DataBlock*>(reinterpret_cast<std::byte*>(addr) - sizeof(DataBlock));
        blockToFree->next = m_freeList;
        m_freeList = blockToFree;
    }

private:
    struct DataBlock
    {
        size_t size;
        DataBlock* next;
    };
    
    DataBlock* m_freeList;
    std::mutex m_mutex;
};
