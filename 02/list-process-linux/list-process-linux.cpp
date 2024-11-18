#include <iostream>
#include <string>
#include <optional>
#include <iomanip>
#include <stdexcept>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

#include "CHandle.h"

using namespace std;

const string DIVIDER = "___________________________________________________________________|";
const int FIRST_COLUMN_WIDTH = 10;
const int SECOND_COLUMN_WIDTH = 40;
const int THIRD_COLUMN_WIDTH = 17;

enum EndCode
{
    Success = 0,
    Error = 1,
};

struct DirDeleter
{
    static void cleanup(DIR* dir)
    {
        if (dir)
        {
            closedir(dir);
        }
    }
};

void PrintTableRow(const string& firstColumn, const string& secondColumn, const string& thirdColumn)
{
    cout << left << setw(FIRST_COLUMN_WIDTH) << firstColumn
        << setw(SECOND_COLUMN_WIDTH) << secondColumn
        << setw(THIRD_COLUMN_WIDTH) << thirdColumn << "|" << endl;
}

struct ProcessInfo
{
    pid_t pid;
    string processName;
    size_t memoryUsageKB;
};

bool GetProcessInfo(pid_t pid, ProcessInfo& processInfo)
{
    processInfo.pid = pid;
    string commPath = "/proc/" + to_string(pid) + "/comm"; // вынести в отдельные функции
    string statusPath = "/proc/" + to_string(pid) + "/status"; // вынести в отдельные функции

    ifstream commFile(commPath);
    if (!commFile.is_open())
    {
        return false;
    }

    string processName;
    getline(commFile, processName);
    commFile.close();
    processInfo.processName = processName;

    ifstream statusFile(statusPath);
    if (!statusFile.is_open())
    {
        return false;
    }

    string line;
    size_t memoryUsageKB;
    while (getline(statusFile, line))
    {
        if (line.compare(0, 6, "VmRSS:") == 0)
        {
            istringstream iss(line);
            string key, value, unit;
            if (iss >> key >> value >> unit)
            {
                try
                {
                    memoryUsageKB = stoi(value);
                }
                catch (const exception& err)
                {
                    return false;
                }
            }
            break;
        }
    }
    processInfo.memoryUsageKB = memoryUsageKB;
    statusFile.close();

    return true;
}

vector<pid_t> GetAllPids()
{
    vector<pid_t> pids;
    CHandle<DIR*, DirDeleter> procDir(opendir("/proc"));
    if (!procDir.isValid())
    {
        throw runtime_error("Failed to open /proc directory.");
    }

    struct dirent* entry;
    while ((entry = readdir(procDir.get())) != nullptr)
    {
        if (entry->d_type == DT_DIR)
        {
            string dirName = entry->d_name;
            if (all_of(dirName.begin(), dirName.end(), ::isdigit))
            {
                try
                {
                    pid_t pid = stoi(dirName);
                    pids.push_back(pid);
                }
                catch (const exception& err)
                {}
            }
        }
    }

    return pids;
}

void ListProcesses()
{
    vector<pid_t> pids = GetAllPids();

    PrintTableRow("PID", "Process name", "Memory usage (KB)");

    cout << DIVIDER << endl;
    for (const auto& pid : pids)
    {
        ProcessInfo processInfo;
        if (GetProcessInfo(pid, processInfo))
        {
            PrintTableRow(to_string(processInfo.pid), processInfo.processName, to_string(processInfo.memoryUsageKB));
        }
    }
    cout << DIVIDER << endl;
}

int main()
{
    try
    {
        ListProcesses();
    }
    catch (exception& error)
    {
        cout << error.what() << endl;
        return EndCode::Error;
    }

    return EndCode::Success;
}
