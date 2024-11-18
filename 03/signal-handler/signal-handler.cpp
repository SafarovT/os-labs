#include <iostream>
#include <fstream>
#include <filesystem>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>
#include "signal-handler.h"

namespace
{
    using namespace std;

    const int RUN_TIME_SECONDS = 100;
    const int DELAY_SECONDS = 1;
    atomic_bool exitRequested(false);

    void SignalHandler(int signal)
    {
        if (signal == SIGINT || signal == SIGTERM)
        {
            cout << "Process " << getpid() << " has received signal #" << signal << endl;
            exitRequested.store(true);
        }
    }

    class TempFile
    {
    public:
        ~TempFile()
        {
            try
            {
                m_ofstream.close();
                if (filesystem::exists(m_filename))
                {
                    filesystem::remove(m_filename);
                    cout << "File " << m_filename << " has been removed.\n";
                }
            }
            catch (const exception& e)
            {
                cout << "Error while deleting file: " << e.what() << endl;
            }
        }

        explicit TempFile(const string& filename)
            : m_filename(filename)
        {
            m_ofstream.open(m_filename);
            if (!m_ofstream.is_open())
            {
                cout << "Failed to create temp file" << endl;
            }
        }

        void Flush()
        {
            if (!m_ofstream.flush())
            {
                throw runtime_error("Failed to work with temp file");
            }
        }

        void Write(int number)
        {
            if (m_ofstream.is_open())
            {
                m_ofstream << number << endl;
            }
        }

    private:
        string m_filename;
        ofstream m_ofstream;
    };
}

int main(int argc, char* argv[])
{
    cout << argv[0] << endl;
    cout << "pid " << getpid() << endl; // выяснить в каком потоке запускается обраточтчик сигнала

    signal(SIGINT, SignalHandler);
    signal(SIGTERM, SignalHandler);

    try
    {
        TempFile tempFile("temp.txt");

        for (auto i = 0; i <= RUN_TIME_SECONDS; ++i)
        {
            if (exitRequested.load())
            {
                cout << "Terminating process..." << endl;
                break;
            }
            cout << i << endl;
            tempFile.Write(i);
            this_thread::sleep_for(chrono::seconds(DELAY_SECONDS));
        }

        tempFile.Flush();
        cout << "Done" << endl;
    }
    catch (exception const& e)
    {
        cerr << e.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
