#pragma once
#include <winsock2.h>

class SocketRAII
{
public:
	SocketRAII() = default;

	SocketRAII(const SocketRAII& p) = delete;
	SocketRAII& operator= (const SocketRAII) = delete;

	SOCKET GetSocket() const
	{
		return m_socket;
	}

	~SocketRAII()
	{
		closesocket(m_socket);
	}

protected:
	SOCKET m_socket;
};

class HostSocketRAII : public SocketRAII
{
public:
	HostSocketRAII(const addrinfo* serverInfo)
	{
		m_socket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
	}
};

class SocketListenerRAII : public SocketRAII
{
public:
	SocketListenerRAII(const SOCKET& listenSocket)
	{
		m_socket = accept(listenSocket, nullptr, nullptr);
	}
};

class WsaRAII
{
public:
	WsaRAII(WSADATA& data)
	{
		bool startupError = WSAStartup(MAKEWORD(2, 2), &data);
		if (startupError)
		{
			throw std::runtime_error("Failed to start a server");
		}
	}

	WsaRAII(const WsaRAII& p) = delete;
	WsaRAII& operator= (const WsaRAII) = delete;

	~WsaRAII()
	{
		WSACleanup();
	}
};

class AddrInfoRAII
{
public:
	AddrInfoRAII(int family, int socketType, int protocol)
	{
		hints.ai_family = family;
		hints.ai_socktype = socketType;
		hints.ai_protocol = protocol;
	}

	AddrInfoRAII(int family, int socketType, int protocol, int flags)
		: AddrInfoRAII(family, socketType, protocol)
	{
		hints.ai_flags = flags;
	}

	AddrInfoRAII(const std::string& address, const std::string& port, int family, int socketType, int protocol)
		: AddrInfoRAII(family, socketType, protocol)
	{
		getaddrinfo(address.c_str(), port.c_str(), &hints, &serverInfo);
	}

	AddrInfoRAII(const std::string& port, int family, int socketType, int protocol, int flags)
		: AddrInfoRAII(family, socketType, protocol, flags)
	{
		getaddrinfo(nullptr, port.c_str(), &hints, &serverInfo);
	}

	AddrInfoRAII(const AddrInfoRAII& p) = delete;
	AddrInfoRAII& operator= (const AddrInfoRAII) = delete;

	~AddrInfoRAII()
	{
		freeaddrinfo(serverInfo);
	}

	addrinfo GetHints() const
	{
		return hints;
	}

	addrinfo* GetServerInfo()
	{
		return serverInfo;
	}

private:
	addrinfo hints = {}, * serverInfo;
};