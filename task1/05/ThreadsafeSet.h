#pragma once
#include <unordered_set>
#include <shared_mutex>
#include <mutex>
#include <vector>

template <typename T, typename H = std::hash<T>, typename Comp = std::equal_to<T>>
class ThreadsafeSet
{
public:
    bool Insert(const T& value)
    {
        std::unique_lock lock(m_mutex);
        return m_set.insert(value).second;
    }

    bool Contains(const T& value) const
    {
        std::shared_lock lock(m_mutex);
        return m_set.find(value) != m_set.end();
    }

    std::vector<T> GetSomeElements(size_t count) const
    {
        std::shared_lock lock(m_mutex);
        std::vector<T> result;
        size_t i = 0;
        for (const auto& elem : m_set)
        {
            if (i++ >= count)
            {
                break;
            }
            result.push_back(elem);
        }
        return result;
    }

    void Reserve(size_t size)
    {
        std::unique_lock lock(m_mutex);
        m_set.reserve(size);
    }

private:
    mutable std::shared_mutex m_mutex;
    std::unordered_set<T, H, Comp> m_set;
};
