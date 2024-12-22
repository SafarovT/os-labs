#include <iostream>
#include <unistd.h>
#include <cstring>
#include <limits.h>
#include <stdexcept>
#include "RAII.h"

namespace
{
    using namespace std;

    int BUFFER_SIZE = (sizeof(struct inotify_event) + NAME_MAX + 1) * 16;

    char* ParseArgs(int argc, char* argv[])
    {
        if (argc < 2)
        {
            throw invalid_argument("Usage file.exe <PATH>");
        }
        return argv[1];
    }

    void PrintEvent(uint32_t mask, const string& fileName)
    {
        cout << "File [" << fileName << "] event: ";

        auto checkEvent = [&mask](uint32_t eventCode, string&& output)
        {
            if (mask & eventCode)
            {
                cout << output;
            }
        };
        
        checkEvent(IN_CREATE, "[CREATE] File or directory created");
        checkEvent(IN_DELETE, "[DELETE] File or directory deleted");
        checkEvent(IN_MODIFY, "[MODIFY] File or directory modified");
        checkEvent(IN_ATTRIB, "[ATTRIB] Metadata of file or directory changed");
        checkEvent(IN_MOVED_FROM, "[MOVED_FROM] File or directory moved out");
        checkEvent(IN_MOVED_TO, "[MOVED_TO] File or directory moved into");
        checkEvent(IN_DELETE_SELF, "[DELETE_SELF] Watched directory deleted");
        checkEvent(IN_MOVE_SELF, "[MOVE_SELF] Watched directory moved");
        cout << endl;
    }

    bool WatchFiles(const char* path)
    {
        InotifyFileDescriptorRAII inotifyFileDescriptorRAII;
        int inotifyFd = inotifyFileDescriptorRAII.GetFileDescriptor();
        if (inotifyFd < 0)
        {
            perror("inotify_init");
            return false;
        }


        WatchDescriptorRAII watchDescriptorRAII(inotifyFd, path);
        int watchDescriptor = watchDescriptorRAII.GetWatchDescriptor();
        if (watchDescriptor < 0)
        {
            perror("inotify_add_watch");
            return false;
        }

        cout << "Monitoring: " << path << endl;

        alignas(alignof(struct inotify_event)) char buffer[BUFFER_SIZE];
        while (true)
        {
            ssize_t length = read(inotifyFd, buffer, sizeof(buffer));
            if (length < 0)
            {
                perror("read");
                break;
            }

            for (char* ptr = buffer; ptr < buffer + length; )
            {
                struct inotify_event* event = reinterpret_cast<struct inotify_event*>(ptr);
                PrintEvent(
                    event->mask,
                    event->len
                        ? event->name
                        : path
                );
                ptr += sizeof(struct inotify_event) + event->len;
            }
        }

        return true;
    }
}

int main(int argc, char* argv[])
{
    try
    {
        const char* path = ParseArgs(argc, argv);
        if (!WatchFiles(path))
        {
            return EXIT_FAILURE;
        }
    }
    catch (const exception& e)
    {
        cout << e.what() << endl;
    }

    
    return EXIT_SUCCESS;
}
