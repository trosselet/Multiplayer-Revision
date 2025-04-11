
#include "NetworkManager.h"
#include <Windows.h>

int main()
{
	NetworkManager* manager = NetworkManager::Get();

	const char* serverIp = "127.0.0.1";
	unsigned int port = 54000;
	const char* playerName = "Player";

	if (manager->Connect(serverIp, port, playerName))
	{
		manager->Run();
	}
	else
	{
		return 1;
	}

	manager->Disconnect();

	return 0;
}