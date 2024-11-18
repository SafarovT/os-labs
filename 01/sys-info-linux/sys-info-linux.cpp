#include <iostream>
#include <string>
#include <stdexcept>
#include <sys/utsname.h>
#include <sys/sysinfo.h>

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
        struct utsname unameData;

        uname(&unameData);

        string sysname = string(unameData.sysname);
        string release = string(unameData.release);

        return sysname + " " + release;
    }

    RamInfo GetRamInfo()
    {
        struct sysinfo info;
        
        if (sysinfo(&info) == -1)
        {
            throw runtime_error("Failed to retrieve system info");
        }

        return
        {
            info.freeram,
            info.totalram,
        };
    }

    unsigned GetProcessorsCount()
    {
        return get_nprocs_conf();
    }
}

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