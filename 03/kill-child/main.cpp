#include <iostream>
#include <vector>
#include <unistd.h>
#include <atomic>
#include <signal.h>
#include <sys/wait.h>
#include <cstring>
#include <stdexcept>
#include <chrono>
#include <thread>

namespace
{
    using namespace std;

    const int DELAY_SECONDS = 5;
    atomic_bool shouldExit(false);

    class ProcessManager
    {
    public:
        pid_t SpawnChild()
        {
            pid_t pid = fork();
            if (pid < 0)
            {
                throw runtime_error("Failed to fork child process");
            }
            else if (pid == 0)
            {
                RunChildProcess();
            }
            else
            {
                childPids.push_back(pid);
            }

            return pid;
        }

        void KillChild()
        {
            if (HasChildren())
            {
                pid_t pid = childPids.back();
                if (KillProcess(pid))
                {
                    cout << "Killed process with PID " << pid << endl;
                    childPids.pop_back();
                }
            }
            else
            {
                cout << "No children to kill" << endl;
            }
        }

        bool HasChildren() const
        {
            return !childPids.empty();
        }

    private:
        vector<pid_t> childPids;

        static void RunChildProcess()
        {
            signal(SIGTERM, HandleSignal);
            signal(SIGINT, HandleSignal);

            while (!shouldExit.load())
            {
                cout << "Child process " << getpid() << " is still alive" << endl;
                sleep(DELAY_SECONDS);
            }
        }

        static void HandleSignal(int signal)
        {
            if (signal == SIGTERM || signal == SIGINT)
            {
                cout << "Child process " << getpid() << " received signal " << signal << ", exiting..." << endl;
                shouldExit.store(true);
            }
        }

        bool KillProcess(pid_t pid)
        {
            if (kill(pid, SIGTERM) < 0)
            {
                cout << "Failed to send SIGTERM to process " << pid << ": " << strerror(errno) << endl;
                return false;
            }

            int status;
            if (waitpid(pid, &status, 0) < 0)
            {
                cout << "Failed to wait for process " << pid << ": " << strerror(errno) << endl;
                return false;
            }
            return true;
        }
    };

    bool HandleCommand(ProcessManager& manager, const string& command)
    {
        if (command == "spawn")
        {
            pid_t pid = manager.SpawnChild();
            if (pid > 0)
            {
                cout << "Child process PID = " << pid << endl;
            }
        }
        else if (command == "kill")
        {
            manager.KillChild();
        }
        else if (command == "exit")
        {
            cout << "The end." << endl;
            return false;
        }
        else
        {
            cout << "Unknown command " << command << endl;
        }

        return true;
    }
}

int main()
{
    ProcessManager manager;
    string command;

    while (true)
    {
        try
        {
            cout << "> ";
            getline(cin, command);
            if (!HandleCommand(manager, command))
            {
                return EXIT_SUCCESS;
            }
        }
        catch (exception const& e)
        {
            cout << e.what() << endl;
            return EXIT_FAILURE;
        }
    }
}
