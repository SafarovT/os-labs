#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <stdexcept>
#include <optional>

enum EndCode
{
    Success = 0,
    Error = 1,
};

using namespace std;

bool IsChildProcess(pid_t pid)
{
    return pid == 0;
}

char FlipCase(char symbol)
{
    if (islower(symbol))
    {
        return toupper(symbol);
    }
    if (isupper(symbol))
    {
        return tolower(symbol);
    }
    return symbol;
}

template <class T>
void OpenFile(T& file, string const& fileName)
{
    file.open(fileName);
    if (!file.is_open())
    {
        throw runtime_error("Failed to open file " + fileName);
    }
}

void CheckFileRuntime(ifstream& inputFile)
{
    if (inputFile.bad())
    {
        throw runtime_error("Failed to work with given file");
    }
}

void CheckFileRuntime(ofstream& outputFile)
{
    if (!outputFile.flush())
    {
        throw runtime_error("Failed to write in output file");
    }
}

pid_t CreateChildProcess()
{
    pid_t pid = fork();

    if (pid < 0)
    {
        throw runtime_error("Ошибка при попытке создать дочерний процесс.");
    }

    return pid;
}

void ProcessFile(const string& inputFileName)
{
    pid_t pid = getpid();
    cout << "Process " << pid << " is processing " << inputFileName;
    string outputFileName = inputFileName + ".out";

    ifstream inputFile;
    ofstream outputFile;

    OpenFile<ifstream>(inputFile, inputFileName);
    OpenFile<ofstream>(outputFile, outputFileName);

    char ch;
    while (inputFile.get(ch))
    {
        outputFile.put(FlipCase(ch));
    }

    CheckFileRuntime(inputFile);
    CheckFileRuntime(outputFile);

    cout << "Process " << pid << " has finished writing to " << outputFileName << endl;
}

void ProcessAllFiles(int argc, char* argv[])
{
    for (int i = 1; i < argc; ++i)
    {
        try
        {
            pid_t pid = CreateChildProcess();
            if (IsChildProcess(pid))
            {
                ProcessFile(argv[i]);
            }
        }
        catch (exception const& e)
        {
            cout << e.what() << endl;
        }
    }
}

void KillAllChildProcesses()
{
    int status;
    pid_t finishedPid = wait(&status);
    while (finishedPid > 0)
    {
        if (WIFEXITED(status))
        {
            cout << "Child process " << finishedPid << " is over" << endl;
        }
        else
        {
            cout << "Child process " << finishedPid << " ended with error" << endl;
        }
        finishedPid = wait(&status);
    }
}

int main(int argc, char* argv[])
{
    ProcessAllFiles(argc, argv);

    KillAllChildProcesses();
    
    return EndCode::Success;
}