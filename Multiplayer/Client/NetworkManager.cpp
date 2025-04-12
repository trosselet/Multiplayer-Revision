#include "NetworkManager.h"

NetworkManager::NetworkManager() : m_criticalSection(), m_ip(0), m_isRunning(false), m_serverAddress(), m_username(0), mp_socket(nullptr),
m_userId(0)
{
	InitializeCriticalSection(&m_criticalSection);
}

NetworkManager::~NetworkManager()
{
	DeleteCriticalSection(&m_criticalSection);
}

bool NetworkManager::Connect(const char* ipAddress, unsigned int port, const char* name)
{
	EnterCriticalSection(&m_criticalSection);

	mp_socket = new CSocket();

	if (!mp_socket->Init(0))
	{
		std::cout << "Failed to init socket on client." << std::endl;
		return false;
	}

	m_ip = ipAddress;

	m_serverAddress = {};
	m_serverAddress.sin_family = AF_INET;
	m_serverAddress.sin_port = htons(port);
	inet_pton(AF_INET, m_ip, &m_serverAddress.sin_addr);

	CSocket::ConnectionRequestPacket requestConnection;
	requestConnection.header.packetType = CSocket::CONNECTION_REQUEST;
	requestConnection.header.userId = 0;
	strncpy_s(requestConnection.name, name, sizeof(requestConnection.name));

	if (!mp_socket->SendTo(m_ip, port, &requestConnection, sizeof(requestConnection)))
	{
		std::cerr << "Failed to send connection request to server. Error: " << WSAGetLastError() << std::endl;
		return false;
	}

	std::cout << "Connection request sent to " << m_ip << ":" << port << std::endl;

	LeaveCriticalSection(&m_criticalSection);

	return true;
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
		m_userId = answer->header.userId;
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
		std::string input;
		std::getline(std::cin, input);

		if (input == "/quit")
		{
			Disconnect();
			return;
		}

		CSocket::SendRequestPacket packet = {};
		packet.header.packetType = CSocket::SEND_REQUEST;
		packet.header.userId = m_userId;
		strcpy_s(packet.name, m_username);
		strcpy_s(packet.message, input.c_str());

		if (!mp_socket->SendTo(m_ip, ntohs(m_serverAddress.sin_port), &packet, sizeof(packet)))
		{
			std::cerr << "Failed to send message to server." << std::endl;
		}
	}
}

void NetworkManager::Disconnect()
{
	if (!mp_socket)
		return;

	CSocket::SendRequestPacket packet = {};
	packet.header.packetType = CSocket::DISCONNECT_REQUEST;
	packet.header.userId = m_userId;
	strcpy_s(packet.name, m_username);
	strcpy_s(packet.message, "Disconnected.");

	if (!mp_socket->SendTo(m_ip, ntohs(m_serverAddress.sin_port), &packet, sizeof(packet)))
	{
		std::cerr << "Failed to send disconnect message to server." << std::endl;
	}

	m_isRunning = false;
}

NetworkManager* NetworkManager::Get()
{
	static NetworkManager mInstance;
	return &mInstance;
}
