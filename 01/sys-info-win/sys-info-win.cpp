#include <iostream>
#include <string>
#include <stdexcept>
#include <windows.h>
#include <VersionHelpers.h>

namespace
{
    using namespace std;

    enum EndCode
    {
        Success = 0,
        Error = 1,
    };

    using MemNumericType = unsigned long long;

    struct RamInfo
    {
        MemNumericType availableCapacity, fullCapacity;
    };

    MemNumericType BytesToMb(MemNumericType bytes)
    {
        return bytes / static_cast<MemNumericType>(1024 * 1024);
    }

    string GetOSVersion()
    {
        auto getResultStr = [](const string& version) { return "Windows " + version + " or Greater"; };
        if (IsWindows10OrGreater()) {
            return getResultStr("10");
        }
        if (IsWindows8OrGreater()) {
            return getResultStr("8");
        }
        if (IsWindows7OrGreater()) {
            return getResultStr("7");
        }
        if (IsWindowsVistaOrGreater()) {
            return getResultStr("Vista");
        }
        if (IsWindowsXPOrGreater()) {
            return getResultStr("XP");
        }

        throw runtime_error("Windows versions below XP are not supported");
    }

    RamInfo GetRamInfo()
    {
        MEMORYSTATUSEX memoryStatus;

        memoryStatus.dwLength = sizeof(memoryStatus);
        if (!GlobalMemoryStatusEx(&memoryStatus))
        {
            throw runtime_error("Failed to retrieve memory data");
        }

        return
        {
            memoryStatus.ullAvailPhys,
            memoryStatus.ullTotalPhys,
        };
    }

    unsigned GetProcessorsCount()
    {
        SYSTEM_INFO sysInfo;

        GetSystemInfo(&sysInfo);

        return sysInfo.dwNumberOfProcessors;
    }
}

// функции с заглавной буквы

int main()
{
    string version;
    unsigned processorsCount = 0;
    RamInfo ramInfo = { 0, 0 };
    try
    {
        version = GetOSVersion();
        processorsCount = GetProcessorsCount();
        ramInfo = GetRamInfo();
    }
    catch (const exception& e)
    {
        cout << e.what() << endl;
        return EndCode::Error;
    };
    ramInfo.availableCapacity = BytesToMb(ramInfo.availableCapacity);
    ramInfo.fullCapacity = BytesToMb(ramInfo.fullCapacity);

    cout
        << "OS: " << version << endl
        << "RAM: " << to_string(ramInfo.availableCapacity) << "MB/" << to_string(ramInfo.fullCapacity) << "MB" << endl
        << "Processors: " << to_string(processorsCount) << endl;

    return EndCode::Success;
}