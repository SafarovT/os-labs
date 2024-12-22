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
        // new
        throw new invalid_argument("Error: No numbers provided.");
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
        throw new invalid_argument("Error: Invalid operation");
    }

    return to_string(result);
}

// string -> число
void RunServer(const string& port)
{
    WSADATA wsaData;
    // RAII WSA
    bool startupError = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (startupError)
    {
        throw new runtime_error("Failed to start a server");
    }

    addrinfo hints = {}, * serverInfo;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(nullptr, port.c_str(), &hints, &serverInfo);

    SocketRAII listenSocketRAII(serverInfo);
    SOCKET listenSocket = listenSocketRAII.GetSocket();
    bind(listenSocket, serverInfo->ai_addr, (int)serverInfo->ai_addrlen);
    // тоже RAII
    freeaddrinfo(serverInfo);

    // убрать listen
    listen(listenSocket, SOMAXCONN);
    cout << "Server is running on port " << port << "..." << endl;

    while (true)
    {
        SocketRAII clientSocketRAII(listenSocket);
        SOCKET clientSocket = clientSocketRAII.GetSocket();
        cout << "Client connected." << endl;

        while (true)
        {
            // протестировать что будет если клиент отправит больше данных
            char buffer[BUFFER_SIZE] = {};
            int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);

            if (bytesReceived > 0)
            {
                string command(buffer, bytesReceived);
                string response = ProcessCommand(command);
                // может ли быть так, что клиент получит данные не полностью
                send(clientSocket, response.c_str(), response.size(), 0);
            }
            else
            {
                cout << "Client disconnected." << endl;
                break;
            }
        }
    }

    WSACleanup();
}

// При отсутсвии параметров сервер не должен падать
// Соединение на каждый сокет обрабатывать отдельно (возможно, в потоке)
// Сделать удобный выход

void RunClient(const string& address, const string& port)
{
    WSADATA wsaData;
    bool startupError = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (startupError)
    {
        throw new runtime_error("Failed to start a client");
    }

    addrinfo hints = {}, * serverInfo;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    getaddrinfo(address.c_str(), port.c_str(), &hints, &serverInfo);

    SOCKET clientSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
    connect(clientSocket, serverInfo->ai_addr, (int)serverInfo->ai_addrlen);
    freeaddrinfo(serverInfo);

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

    closesocket(clientSocket);
    WSACleanup();
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
