#include "sys-info-crossplatform.h"

namespace
{
    using namespace std;

    enum EndCode
    {
        Success = 0,
        Error = 1,
    };

    SysInfo::MemNumericType BytesToMb(SysInfo::MemNumericType bytes)
    {
        return bytes / static_cast<SysInfo::MemNumericType>(1024 * 1024);
    }
}

int main()
{
    SysInfo sysInfo;
    string system;
    SysInfo::MemNumericType freeMemory;
    SysInfo::MemNumericType totalMemory;
    unsigned processorsCount;

    try
    {
        system = sysInfo.GetOSName() + " " + sysInfo.GetOSVersion();
        freeMemory = BytesToMb(sysInfo.GetFreeMemory());
        totalMemory = BytesToMb(sysInfo.GetTotalMemory());
        processorsCount = sysInfo.GetProcessorCount();
    }
    catch (const exception& e)
    {
        cout << e.what() << endl;
        return EndCode::Error;
    };

    cout
        << "OS: " << system << endl
        << "RAM: " << to_string(freeMemory) << "MB/" << to_string(totalMemory) << "MB" << endl
        << "Processors: " << to_string(processorsCount) << endl;

    return EndCode::Success;
}