#include "CSocket.h"
#include <iostream>
#include <winsock2.h>

#pragma comment(lib, "Ws2_32.lib")

CSocket::CSocket()
{
}

CSocket::~CSocket()
{
    WSACleanup();
}

bool CSocket::Init(unsigned int port)
{
    WSADATA wsadata;
    int result = WSAStartup(MAKEWORD(2, 2), &wsadata);

    if (result != 0)
    {
        std::cerr << "WSAStartup failed with error: " << result << std::endl;
        return false;
    }

    m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_socket == INVALID_SOCKET)
    {
        std::cerr << "Failed to create socket, error: " << WSAGetLastError() << std::endl;
        return false;
    }

    m_inputAddress = {};
    m_inputAddress.sin_family = AF_INET;
    m_inputAddress.sin_port = htons(port);
    m_inputAddress.sin_addr.S_un.S_addr = INADDR_ANY;

    result = bind(m_socket, (sockaddr*)&m_inputAddress, sizeof(m_inputAddress));
    if (result == SOCKET_ERROR)
    {
        std::cerr << "Failed to bind socket, error: " << WSAGetLastError() << std::endl;
        closesocket(m_socket);
        return false;
    }

    return true;
}

bool CSocket::SendTo(const char* ip, unsigned int port, const void* data, int dataSize)
{
    sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_port = htons(port);
    int inetResult = inet_pton(AF_INET, ip, &dest.sin_addr);
    if (inetResult <= 0)
    {
        std::cerr << "Invalid IP address format." << std::endl;
        return false;
    }

    int result = sendto(m_socket, static_cast<const char*>(data), dataSize, 0, (sockaddr*)&dest, sizeof(dest));
    if (result == SOCKET_ERROR)
    {
        std::cerr << "sendto failed with error: " << WSAGetLastError() << std::endl;
        return false;
    }

    return true;
}

bool CSocket::ReceiveFrom(void* buffer, int bufferSize, sockaddr_in& fromAddress)
{
    int fromSize = sizeof(fromAddress);
    int result = recvfrom(m_socket, static_cast<char*>(buffer), bufferSize, 0, (sockaddr*)&fromAddress, &fromSize);

    if (result == SOCKET_ERROR)
    {
        std::cerr << "recvfrom failed with error: " << WSAGetLastError() << std::endl;
        return false;
    }

    return true;
}
