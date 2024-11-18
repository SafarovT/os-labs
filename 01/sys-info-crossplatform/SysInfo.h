#pragma once
#include <string>

class SysInfo
{
public:
    using MemNumericType = unsigned long long;

    std::string GetOSName() const;
    std::string GetOSVersion() const;
    MemNumericType GetFreeMemory() const;
    MemNumericType GetTotalMemory() const;
    unsigned GetProcessorCount() const;
};
