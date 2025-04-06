#include "NetworkManager.h"

bool NetworkManager::Connect(const char* ipAddress, unsigned int port, const char* name)
{
	EnterCriticalSection(&m_criticalSection);

	mp_socket = new CSocket();

	if (mp_socket->Init(0))
	{
		m_ip = ipAddress;

		m_serverAddress = {};
		m_serverAddress.sin_family = AF_INET;
		m_serverAddress.sin_port = htons(port);
		inet_pton(AF_INET, m_ip, &m_serverAddress.sin_addr);

		CSocket::ConnectionRequestPacket requestConnection;
		requestConnection.header.packetType = CSocket::CONNECTION_REQUEST;
		requestConnection.header.userId = 0;
		strncpy_s(requestConnection.name, name, sizeof(requestConnection));

		char buffer[40];

		memcpy(buffer, &requestConnection.header.packetType, 4);
		memcpy(buffer, &requestConnection.header.userId, 8);
		memcpy(buffer, &requestConnection.name, 32);

		if (mp_socket->SendTo(m_ip, ntohs(m_serverAddress.sin_port), buffer, sizeof(buffer)))
		{
			return true;
		}

		return false;
	}

	return false;
}

void NetworkManager::HandleServerPackets(const void* packet)
{
	const CSocket::Header* header = static_cast<const CSocket::Header*>(packet);

	switch (header->packetType)
	{
	case CSocket::CONNECTION_ACCEPTED :
	{
		const CSocket::ConnectionAcceptedPacket* answer = static_cast<const CSocket::ConnectionAcceptedPacket*>(packet);
		m_username = answer->name;
		std::cout << answer->name << ": " << answer->message << std::endl;

		break;
	}
	case CSocket::SEND_REQUEST :
	{
		const CSocket::SendRequestPacket* answer = static_cast<const CSocket::SendRequestPacket*>(packet);
		std::cout << answer->name << ": " << answer->message << std::endl;
		break;
	}

	default:
		break;
	}
}

void NetworkManager::Run()
{
	m_isRunning = true;

	CreateThread(nullptr, 0, &NetworkManager::ReceiveThread, this, 0, nullptr);

	while (m_isRunning)
	{

	}
}

NetworkManager* NetworkManager::Get()
{
	static NetworkManager mInstance;
	return &mInstance;
}
