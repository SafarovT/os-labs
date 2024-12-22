#pragma once
#include <winsock2.h>

// SocketRAII - в два класса
class SocketRAII
{
public:
	SocketRAII(addrinfo const* serverInfo)
	{
		m_socket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
		// не использовать c-style cast
		bind(m_socket, serverInfo->ai_addr, (int)serverInfo->ai_addrlen);
		listen(m_socket, SOMAXCONN);
	}

	SocketRAII(const SOCKET& listenSocket)
	{
		m_socket = accept(listenSocket, nullptr, nullptr);
	}

	SocketRAII(const SocketRAII& p) = delete;
	SocketRAII& operator= (const SocketRAII) = delete;

	~SocketRAII()
	{
		closesocket(m_socket);
	}

	// не INT
	int GetSocket() const
	{
		return m_socket;
	}

private:
	SOCKET m_socket;

};