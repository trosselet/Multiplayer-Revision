#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

class CSocket
{
public:
	CSocket();
	virtual ~CSocket();

	bool Init(unsigned int port);
	bool SendTo(const char* ip, unsigned int port, const void* data, int dataSize);
	bool ReceiveFrom(void* buffer, int bufferSize, sockaddr_in& fromAddress);

private:
	SOCKET m_socket;
	sockaddr_in m_inputAddress;

public:

	enum PacketType
	{
		CONNECTION_REQUEST,
		CONNECTION_ACCEPTED,
		SEND_REQUEST
	};

	struct Header
	{
		PacketType packetType;
		unsigned int userId;
	};

	struct ConnectionRequestPacket
	{
		Header header;
		char name[32];
	};

	struct ConnectionAcceptedPacket
	{
		Header header;
		char name[32];
		char message[1024];
	};

	struct SendRequestPacket
	{
		Header header;
		char name[32];
		char message[1024];
	};


};

