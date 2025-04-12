#include "Server.h"

Server::Server() : m_socket(), m_criticalSection(), m_nextUserId(1)
{

}

Server::~Server()
{
	DeleteCriticalSection(&m_criticalSection);
}

void Server::Start(unsigned int port)
{
	InitializeCriticalSection(&m_criticalSection);

	if (!m_socket.Init(port))
	{
		std::cout << "Failed to initialize server socket !" << std::endl;
		return;
	}

	CreateThread(nullptr, 0, &Server::ReceiverThread, this, 0, nullptr);


}

void Server::HandlePacket(const char* packet, const sockaddr_in& from)
{
	const CSocket::Header* header = reinterpret_cast<const CSocket::Header*>(packet);

	std::cout << "Packet type: " << header->packetType << std::endl;

	switch (header->packetType)
	{
	case CSocket::CONNECTION_REQUEST:
	{
		const CSocket::ConnectionRequestPacket* req = reinterpret_cast<const CSocket::ConnectionRequestPacket*>(packet);
		std::cout << "New client: " << req->name << std::endl;

		unsigned int userId = m_nextUserId++;
		m_clients[userId] = from;

		CSocket::ConnectionAcceptedPacket response = {};
		response.header.packetType = CSocket::CONNECTION_ACCEPTED;
		response.header.userId = userId;
		strcpy_s(response.name, req->name);
		strcpy_s(response.message, "has connected on the server");


		char ipStr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(from.sin_addr), ipStr, INET_ADDRSTRLEN);

		if (!m_socket.SendTo(ipStr, ntohs(from.sin_port), &response, sizeof(response)))
		{
			std::cerr << "Failed to send CONNECTION_ACCEPTED to " << ipStr << ":" << ntohs(from.sin_port) << std::endl;
		}
		else
		{
			std::cout << "Successfully sent CONNECTION_ACCEPTED to " << ipStr << ":" << ntohs(from.sin_port) << std::endl;
		}

		break;
	}
	case CSocket::SEND_REQUEST:
	{
		const CSocket::SendRequestPacket* req = reinterpret_cast<const CSocket::SendRequestPacket*>(packet);

		std::cout << req->name << ": " << req->message << std::endl;

		char ipStr[INET_ADDRSTRLEN];
		
		for (auto& addr : m_clients)
		{
			
			inet_ntop(AF_INET, &(addr.second.sin_addr), ipStr, INET_ADDRSTRLEN);

			if (!m_socket.SendTo(ipStr, ntohs(addr.second.sin_port), req, sizeof(*req)))
			{
				std::cerr << "Failed to send message to " << ipStr << ":" << ntohs(addr.second.sin_port) << std::endl;
			}
			else
			{
				std::cout << "Successfully sent message to " << ipStr << ":" << ntohs(addr.second.sin_port) << std::endl;
			}
		}

		break;
	}
	case CSocket::DISCONNECT_REQUEST:
	{
		unsigned int userId = header->userId;

		auto it = m_clients.find(userId);
		if (it != m_clients.end())
		{
			std::cout << "User ID " << userId << " disconnected." << std::endl;
			m_clients.erase(it);
		}

		const CSocket::SendRequestPacket* req = reinterpret_cast<const CSocket::SendRequestPacket*>(packet);

		std::cout << req->name << ": disconnect from the chat" << std::endl;

		char ipStr[INET_ADDRSTRLEN];

		for (auto& addr : m_clients)
		{

			inet_ntop(AF_INET, &(addr.second.sin_addr), ipStr, INET_ADDRSTRLEN);

			if (!m_socket.SendTo(ipStr, ntohs(addr.second.sin_port), req, sizeof(*req)))
			{
				std::cerr << "Failed to send message to " << ipStr << ":" << ntohs(addr.second.sin_port) << std::endl;
			}
			else
			{
				std::cout << "Successfully sent message to " << ipStr << ":" << ntohs(addr.second.sin_port) << std::endl;
			}
		}

		break;
	}

	default:
		break;
	}
}
