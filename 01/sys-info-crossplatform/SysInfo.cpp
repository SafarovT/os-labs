#include "SysInfo.h"
#include <stdexcept>

namespace
{
	using namespace std;
}

#if defined(_WIN32)
#include <windows.h>
#include <VersionHelpers.h>

std::string SysInfo::GetOSName() const
{
    return "Windows";
}

std::string SysInfo::GetOSVersion() const
{
    auto getResultStr = [](const string& version) { return version + " or Greater"; };
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

MEMORYSTATUSEX GetMemoryStatus()
{
    MEMORYSTATUSEX memoryStatus;

    memoryStatus.dwLength = sizeof(memoryStatus);
    if (!GlobalMemoryStatusEx(&memoryStatus))
    {
        throw runtime_error("Failed to retrieve memory data");
    }
    
    return memoryStatus;
}

SysInfo::MemNumericType SysInfo::GetFreeMemory() const
{
    MEMORYSTATUSEX memoryStatus = GetMemoryStatus();

    return memoryStatus.ullAvailPhys;
}

SysInfo::MemNumericType SysInfo::GetTotalMemory() const
{
    MEMORYSTATUSEX memoryStatus = GetMemoryStatus();

	return memoryStatus.ullTotalPhys;
}

unsigned SysInfo::GetProcessorCount() const
{
    SYSTEM_INFO sysInfo;

    GetSystemInfo(&sysInfo);

    return sysInfo.dwNumberOfProcessors;
}

#elif __linux__
#include "SysInfo.h"
#include <sys/utsname.h>
#include <sys/sysinfo.h>

std::string SysInfo::GetOSName() const
{
    struct utsname unameData;
    uname(&unameData);

    string sysname = string(unameData.sysname);

    return sysname;
}

std::string SysInfo::GetOSVersion() const
{
    struct utsname unameData;
    uname(&unameData);

    string release = string(unameData.release);

    return release;
}

struct sysinfo GetSysInfo()
{
    struct sysinfo info;

    if (sysinfo(&info) == -1)
    {
        throw runtime_error("Failed to retrieve system info");
    }

    return info;
}

SysInfo::MemNumericType SysInfo::GetFreeMemory() const
{
    struct sysinfo info = GetSysInfo();

    return info.freeram;
}

SysInfo::MemNumericType SysInfo::GetTotalMemory() const
{
    struct sysinfo info = GetSysInfo();
    
    return info.totalram;
}

unsigned SysInfo::GetProcessorCount() const
{
    return get_nprocs_conf();
}

#endif