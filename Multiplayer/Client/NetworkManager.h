#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

#include <Windows.h>

#include "CSocket.h"


class NetworkManager
{
public:
	NetworkManager() = default;
	virtual ~NetworkManager() = default;

	bool Connect(const char* ipAddress, unsigned int port, const char* name);
	void HandleServerPackets(const void* packet);

	void Run();

	static NetworkManager* Get();

private:
	inline static DWORD WINAPI ReceiveThread(LPVOID param)
	{
		NetworkManager* manager = static_cast<NetworkManager*>(param);
		char buffer[2048];
		sockaddr_in inputAddress;

		while (manager->m_isRunning)
		{
			if (manager->mp_socket->ReceiveFrom(buffer, sizeof(buffer), inputAddress))
			{
				EnterCriticalSection(&manager->m_criticalSection);
				manager->HandleServerPackets(buffer);
				LeaveCriticalSection(&manager->m_criticalSection);
			}
		}

		return 0;

	}

public:
	bool m_isRunning;
	CSocket* mp_socket;

	const char* m_username;

private:
	CRITICAL_SECTION m_criticalSection;

	const char* m_ip;
	sockaddr_in m_serverAddress;
};

