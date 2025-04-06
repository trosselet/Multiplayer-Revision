#include "CSocket.h"

CSocket::CSocket()
{
}

CSocket::~CSocket()
{
}

bool CSocket::Init(unsigned int port)
{
    WSADATA wsadata;
    WSAStartup(MAKEWORD(2, 2), &wsadata);

    m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    m_inputAddress = {};
    m_inputAddress.sin_family = AF_INET;
    m_inputAddress.sin_port = htons(port);
    m_inputAddress.sin_addr.S_un.S_addr = INADDR_ANY;

    HRESULT hr = bind(m_socket, (sockaddr*)&m_inputAddress, sizeof(m_inputAddress));

    if (hr != S_OK)
    {
        return false;
    }

    return true;
}

bool CSocket::SendTo(const char* ip, unsigned int port, const void* data, int dataSize)
{
    sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_port = htons(port);
    inet_pton(dest.sin_family, ip, &dest.sin_addr);

    HRESULT hr = sendto(m_socket, static_cast<const char*>(data), dataSize, 0, (sockaddr*)&dest, sizeof(dest));

    if (hr != S_OK)
    {
        return false;
    }

    return true;
}

bool CSocket::ReceiveFrom(void* buffer, int bufferSize, sockaddr_in& fromAddress)
{
    int fromSize = sizeof(fromAddress);
    HRESULT hr = recvfrom(m_socket, static_cast<char*>(buffer), bufferSize, 0, (sockaddr*)&fromAddress, sizeof(fromAddress));

    if (hr != S_OK)
    {
        return false;
    }

    return true;
}
