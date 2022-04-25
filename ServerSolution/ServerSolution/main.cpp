#include <iostream>
#include <WS2tcpip.h>
#include <sstream>
#pragma comment (lib, "ws2_32.lib")
void main()
{
    //Initializing winsock
    WSADATA data;
    WORD version = MAKEWORD(2, 2);

    int wsok = WSAStartup(version, &data);
    if (wsok != 0)
    {
        std::cerr << "Unable to initialize winsock." << std::endl;
        return;
        // Create socket to be bound to
        SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
        if (listening == INVALID_SOCKET)
        {
            std::cerr << "Unable to create socket" << std::endl;
            return;
        }

        // Bind socket to ip and port
        sockaddr_in adressIn;
        adressIn.sin_family = AF_INET;
        adressIn.sin_port = htons(54000);
        adressIn.sin_addr.S_un.S_addr = INADDR_ANY;

        int result = bind(listening, (sockaddr*)&adressIn, sizeof(adressIn));
        // error checking

        // This socket for listening
        listen(listening, SOMAXCONN);

        // Wait for connection
        sockaddr_in client;
        int clientSize = sizeof(client);

        SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
        /*if (clientSocket == INVALID_SOCKET)
        {
            if (WSAGetLastError() == WSAESHUTDOWN)
            {
                return shutdown();
            }
        }*/

        char host[NI_MAXHOST];
        char service[NI_MAXHOST];

        ZeroMemory(host, NI_MAXHOST);
        ZeroMemory(service, NI_MAXHOST);

        if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
        {
            std::cout << host << " connected on port " << service << std::endl;
        }
        else
        {
            inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
            std::cout << host << " connected on port " << ntohs(client.sin_port) << std::endl;
        }

        // Close listening socket
        closesocket(listening);

        // Accept message & echo back (will be removed)
        std::cout << "The Server is now listening for clients" << std::endl;

        fd_set masterSet;
        FD_ZERO(&masterSet);
        FD_SET(listening, &masterSet);

        static int clients = 0;
        clients = clients + 1;
        while (true)
        {
            fd_set temp = masterSet;

            int socketCount = select(temp.fd_count, &temp, nullptr, nullptr, nullptr);

            for (int i = 0; i < socketCount; i++)
            {
                SOCKET sock = temp.fd_array[i];

                if (sock == listening)
                {

                    // can get info from the client
                    SOCKET client = accept(listening, nullptr, nullptr);

                    FD_SET(client, &masterSet);

                    // can send to the new client
                    std::string welcome = "Welcome to the server\n";
                    send(client, welcome.c_str(), welcome.size() + 1, 0);
                }
                else
                {
                    char buf[4096];
                    ZeroMemory(buf, 4096);


                    int bytesIn = recv(sock, buf, 4096, 0);
                    if (bytesIn <= 0)
                    {
                        // Drop Client
                        closesocket(sock);
                        FD_CLR(sock, &masterSet);
                    }
                    else
                    {
                        for (int i = 0; i < masterSet.fd_count; i++)
                        {
                            SOCKET outSock = masterSet.fd_array[i];

                            std::ostringstream ss;
                            ss << "SOCKET #" << sock << ": " << buf << "\n";

                            std::string stringOut = ss.str();

                            send(outSock, buf, bytesIn, 0);
                        }

                        std::cout << "SOCKET #" << sock << ": " << buf << "\n";
                    }
                }
            }
            // Closing sock
            //closesocket(clientSocket);

            // Cleanup Winsock
        }
            WSACleanup();
    }
}














        //while (true)
        //{
        //    ZeroMemory(buf, 4906);

        //    // Wait for data to arrive
        //    int bytesrecieved = recv(clientSocket, buf, 4906, 0);
        //    if (bytesrecieved == SOCKET_ERROR)
        //    {
        //        std::cerr << "error in recv" << std::endl;
        //        break;
        //    }
        //    if (bytesrecieved == 0)
        //    {
        //        std::cout << "Client disconenected." << std::endl;
        //        break;
        //    }
        //    // Echo message back to client 
        //    send(clientSocket, buf, bytesrecieved + 1, 0);

        //}