#pragma once

template <typename HandleType, typename Deleter>
class CHandle {
public:
    explicit CHandle(HandleType handle = nullptr) : m_handle(handle) {}

    ~CHandle() {
        if (m_handle) {
            Deleter::cleanup(m_handle);
        }
    }

    CHandle(const CHandle&) = delete;
    CHandle& operator=(const CHandle&) = delete;

    CHandle(CHandle&& other) noexcept : m_handle(other.m_handle) {
        other.m_handle = nullptr;
    }

    CHandle& operator=(CHandle&& other) noexcept {
        if (this != &other) {
            if (m_handle) {
                Deleter::cleanup(m_handle);
            }
            m_handle = other.m_handle;
            other.m_handle = nullptr;
        }
        return *this;
    }

    HandleType get() const { return m_handle; }

    HandleType release() {
        HandleType temp = m_handle;
        m_handle = nullptr;
        return temp;
    }

    bool isValid() const {
        return m_handle != nullptr;
    }

private:
    HandleType m_handle;
};

