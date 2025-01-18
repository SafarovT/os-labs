#include "Server.h"
#include "Client.h"
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        int port = std::atoi(argv[1]);
        RunServer(port);
    }
    else if (argc == 3)
    {
        std::string address = argv[1];
        int port = std::atoi(argv[2]);
        RunClient(address, port);
    }
    else
    {
        std::cerr << "Usage:\n"
            << "  Server mode: whiteboard <port>\n"
            << "  Client mode: whiteboard <address> <port>\n";
        return 1;
    }

    return 0;
}