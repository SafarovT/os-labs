#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <stdexcept>
#include <string>

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

pid_t CreateChildProcess()
{
    pid_t pid = fork();

    if (pid < 0)
    {
        throw runtime_error("Ошибка при попытке создать дочерний процесс.");
    }
    
    return pid;
}

int ReadNumber()
{
    string input;
    int pid;

    try
    {
        getline(cin, input);
        pid = stoi(input);
        return pid;
    }
    catch (exception& e)
    {
        cout << "Введен некорректный формат PID" << endl;
    }
    return 0;
}

pid_t WaitProcess(pid_t pid)
{
    int status;
    pid_t finishedPid = waitpid(pid, &status, 0);

    if (!WIFEXITED(status) || finishedPid == -1)
    {
        cout << "Ошибка при вызове waitpid" << endl;
    }
    else
    {
        cout << "Дочерний процесс завершен" << endl;
    }

    return finishedPid;
}

void KillChildProcess(pid_t pid)
{
    pid_t finishedPid = -1;

    cout << "Введите PID дочернего процесса:" << endl; // выяснить почему при -1
    while (finishedPid == -1)
    {
        int inputPid = ReadNumber();

       if (inputPid == pid)
       {
           finishedPid = WaitProcess(inputPid);
       }
       else
       {
           cout << "Введен неверный PID. Попробуйте снова:" << endl;
       }
    }
}

int main() {
    pid_t processPid;
    try
    {
        processPid = CreateChildProcess();
    }
    catch (std::exception const& e)
    {
        cout << "ERROR" << endl <<  e.what() << endl;
        return EndCode::Error;
    }

    if (IsChildProcess(processPid))
    {
        cout << "PID дочернего процесса: " << getpid() << endl;
    }
    else
    {
        KillChildProcess(processPid);
    }

    return EndCode::Success;
}
