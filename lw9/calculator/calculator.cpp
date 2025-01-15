#pragma comment(lib, "Ws2_32.lib")

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdexcept>
#include "RAII.h"
#include <thread>

using namespace std;

const int BUFFER_SIZE = 1024;

string ProcessCommand(const string& command)
{
    istringstream iss(command);
    char operation;
    iss >> operation;

    vector<int> numbers;
    int num;
    while (iss >> num)
    {
        numbers.push_back(num);
    }

    if (numbers.empty())
    {
        throw invalid_argument("Error: No numbers provided.");
    }

    int result = numbers[0];
    if (operation == '+')
    {
        for (size_t i = 1; i < numbers.size(); ++i)
        {
            result += numbers[i];
        }
    }
    else if (operation == '-')
    {
        for (size_t i = 1; i < numbers.size(); ++i)
        {
            result -= numbers[i];
        }
    }
    else
    {
        throw invalid_argument("Error: Invalid operation");
    }

    return to_string(result);
}

void HandleClient(SOCKET clientSocket)
{
    while (true)
    {
        char buffer[BUFFER_SIZE] = {};
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);

        if (bytesReceived > 0)
        {
            string command(buffer, bytesReceived);
            string response;
            try
            {
                response = ProcessCommand(command);
            }
            catch (const exception& e)
            {
                response = e.what();
            }

            send(clientSocket, response.c_str(), response.size(), 0);
        }
        else
        {
            cout << "Client disconnected." << endl;
            break;
        }
    }
}   

void RunServer(const string& port)
{
    WSADATA wsaData;
    WsaRAII wsaRAII(wsaData);

    AddrInfoRAII addrInfoRAII(port, AF_INET, SOCK_STREAM, SOCK_STREAM, AI_PASSIVE);
    addrinfo hints = addrInfoRAII.GetHints(), * serverInfo = addrInfoRAII.GetServerInfo();

    HostSocketRAII listenSocketRAII(serverInfo);
    SOCKET listenSocket = listenSocketRAII.GetSocket();
    bind(listenSocket, serverInfo->ai_addr, static_cast<int>(serverInfo->ai_addrlen));
    listen(listenSocket, SOMAXCONN);

    cout << "Server is running on port " << port << "..." << endl;

    while (true)
    {
        SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET)
        {
            continue;
        }
        cout << "Client connected." << endl;
        thread(HandleClient, clientSocket).detach();
    }
}

void RunClient(const string& address, const string& port)
{
    WSADATA wsaData;
    WsaRAII wsaRAII(wsaData);

    AddrInfoRAII addrInfoRAII(address, port, AF_INET, SOCK_STREAM, IPPROTO_TCP);
    addrinfo hints = addrInfoRAII.GetHints(), * serverInfo = addrInfoRAII.GetServerInfo();

    HostSocketRAII clientSocketRAII(serverInfo);
    auto clientSocket = clientSocketRAII.GetSocket();
    connect(clientSocket, serverInfo->ai_addr, static_cast<int>(serverInfo->ai_addrlen));

    cout << "Connected to server at " << address << ":" << port << endl;

    while (true)
    {
        string command;
        cout << "> ";
        getline(cin, command);

        if (command.empty())
        {
            continue;
        }
        else if (command == "exit")
        {
            cout << "The end." << endl;
            break;
        }

        send(clientSocket, command.c_str(), command.size(), 0);

        char buffer[BUFFER_SIZE] = {};
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);

        if (bytesReceived > 0)
        {
            cout << "Result: " << buffer << endl;
        }
        else
        {
            cout << "Connection closed by server." << endl;
            break;
        }
    }
}

int main(int argc, char* argv[])
{
    try
    {
        if (argc == 2)
        {
            string port = argv[1];
            RunServer(port);
        }
        else if (argc == 3)
        {
            string address = argv[1];
            string port = argv[2];
            RunClient(address, port);
        }
        else
        {
            cout << "Usage:" << endl
                << "1) Server: calc <PORT>" << endl
                << "2) Client: calc <ADDRESS> <PORT>" << endl;
            return EXIT_FAILURE;
        }
    }
    catch (const exception& e)
    {
        cout << e.what() << endl;
    }

    return EXIT_SUCCESS;
}
