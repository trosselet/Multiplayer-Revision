
#include <iostream>

#include "Server.h"

int main()
{
    unsigned int port = 54000;

    Server server;

    std::cout << "Demarrage du serveur sur le port " << port << "..." << std::endl;

    server.Start(port);

    std::cout << "Serveur en cours d'execution. Appuyez sur Entree pour arr�ter le serveur." << std::endl;

    std::cin.get();

    return 0;
}

