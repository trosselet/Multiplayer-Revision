#pragma once
#include <map>
#include <vector>
#include <string>

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

#include "CSocket.h"

class Server
{
public:
	Server();
	virtual ~Server();

	void Start(unsigned int port);
	void HandlePacket(const char* packet, const sockaddr_in& from);

private:
	CSocket m_socket;
	CRITICAL_SECTION m_criticalSection;
	std::map<unsigned int, sockaddr_in> m_clients;
	unsigned int m_nextUserId;

private:

	static DWORD WINAPI ReceiverThread(LPVOID param)
	{
		Server* server = static_cast<Server*>(param);
		char buffer[2048];
		sockaddr_in from;

		while (true)
		{
			if (server->m_socket.ReceiveFrom(buffer, sizeof(buffer), from))
			{
				EnterCriticalSection(&server->m_criticalSection);
				server->HandlePacket(buffer, from);
				LeaveCriticalSection(&server->m_criticalSection);
			}
		}

		return 0;
	}
};

