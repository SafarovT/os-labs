#include <iostream>
#include <string>
#include <optional>
#include <iomanip>
#include <stdexcept>
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <tchar.h>

using namespace std;

auto const DIVIDER = "___________________________________________________________________|";
const int FIRST_COLUMN_WIDTH = 10;
const int SECOND_COLUMN_WIDTH = 40;
const int THIRD_COLUMN_WIDTH = 17;

enum EndCode
{
    Success = 0,
    Error = 1,
};

void PrintTableRow(const string& firstColumn, const string& secondColumn, const string& thirdColumn)
{
    cout << left << setw(FIRST_COLUMN_WIDTH) << firstColumn
        << setw(SECOND_COLUMN_WIDTH) << secondColumn
        << setw(THIRD_COLUMN_WIDTH) << thirdColumn << "|" << endl;
}

struct ProcessInfo
{
    DWORD pid;
    string processName;
    size_t memoryUsageKB;
};

void InitHandleProcessSnap(HANDLE& handleProcessSnap)
{
    handleProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (handleProcessSnap == INVALID_HANDLE_VALUE)
    {
        throw runtime_error("Failed to take snapshot of processes.");
    }
}

void InitProcessEntry(PROCESSENTRY32& processEntry, HANDLE const& handleProcessSnap)
{
    processEntry.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(handleProcessSnap, &processEntry))
    {
        throw runtime_error("Failed to retrieve information about the processes.");
    }
}

optional<ProcessInfo> GetProcessInfo(PROCESSENTRY32 const& processEntry)
{
    DWORD pid = processEntry.th32ProcessID;
    HANDLE handleProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, pid);

    if (handleProcess == NULL)
    {
        return nullopt;
    }

    TCHAR processName[MAX_PATH] = TEXT("<unknown>");
    HMODULE hMod;
    DWORD cbNeeded;

    if (EnumProcessModules(handleProcess, &hMod, sizeof(hMod), &cbNeeded))
    {
        GetModuleBaseName(handleProcess, hMod, processName, sizeof(processName) / sizeof(TCHAR));
    }

    PROCESS_MEMORY_COUNTERS pmc;
    optional<ProcessInfo> processInfoRes = nullopt;

    if (GetProcessMemoryInfo(handleProcess, &pmc, sizeof(pmc)))
    {
        size_t memoryUsageKB = pmc.WorkingSetSize / 1024;

        processInfoRes = 
        {
            pid,
            processName,
            memoryUsageKB,
        };
    }

    CloseHandle(handleProcess);

    return processInfoRes;
}

void ListProcesses()
{
    HANDLE handleProcessSnap;
    PROCESSENTRY32 processEntry;
    InitHandleProcessSnap(handleProcessSnap);
    InitProcessEntry(processEntry, handleProcessSnap);

    PrintTableRow("PID", "Process name", "Memory usage (KB)");

    cout << DIVIDER << endl;
    do
    {
        auto processInfo = GetProcessInfo(processEntry);
        if (processInfo != nullopt)
        {
            PrintTableRow(to_string(processInfo->pid), processInfo->processName, to_string(processInfo->memoryUsageKB));
        }
    } while (Process32Next(handleProcessSnap, &processEntry));
    cout << DIVIDER << endl;

    CloseHandle(handleProcessSnap);
}

int main()
{
    try
    {
        ListProcesses();
    }
    catch (const exception& error)
    {
        cout << error.what() << endl;
        return EndCode::Error;
    }

    return EndCode::Success;
}