#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include "Queue/queue.h"

const int PORT = 8080;
const int MAX_CLIENTS = 5;

int fibonacci(int n) {
    if (n <= 0) return 0;
    if (n == 1) return 1;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

int main() {
    int serverSocket, clientSockets[MAX_CLIENTS];
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    fd_set readfds;

    // Initialize client sockets to -1 (i.e unused)
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        clientSockets[i] = -1;
    }

    // Create server socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Socket creation failed");
        return 1;
    }

    // Make the server socket non-blocking
    fcntl(serverSocket, F_SETFL, O_NONBLOCK);

    // Initialize server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET; //IPV4
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(PORT); //Port 8080

    // Bind socket to network address (IP + Port)
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Binding failed");
        return 1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, MAX_CLIENTS) == -1) {
        perror("Listen failed");
        return 1;
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    while (true) {
        FD_ZERO(&readfds);

        //adds server socket to readfds
        FD_SET(serverSocket, &readfds);

        // Add client sockets to the file descriptor set
        int maxfd = serverSocket;
        for (int i = 0; i < MAX_CLIENTS; ++i) {

            //if clientSocket[i] is in use
            if (clientSockets[i] != -1) {

                //add client socket to readfds
                //and make it so that 'select' will monitor this socket for incoming data
                FD_SET(clientSockets[i], &readfds);

                //if new clientSocket value is higher
                if (clientSockets[i] > maxfd) {

                    //update maxfd so 'select' checks all relevant client sockets
                    //up to the highest file descriptor number
                    maxfd = clientSockets[i];
                }
            }
        }

        // Wait for activity on sockets
        select(maxfd + 1, &readfds, NULL, NULL, NULL);

        // Check for incoming connection on SERVER SOCKET
        // if present, it would've been added to read fds in the for loop above
        if (FD_ISSET(serverSocket, &readfds)) {
	
	    //accept it in clientSocket
            int clientSocket = accept(serverSocket, (struct sockaddr*) &clientAddr, &clientAddrLen);

            // Make the client socket non-blocking
            if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1) {
		            perror("Setting socket to non-blocking mode failed");
		            close(clientSocket);
		            return 1;
	          }

            // Add client socket to first non-empty spot in the array
            for (int i = 0; i < MAX_CLIENTS; ++i) {
                if (clientSockets[i] == -1) {
                    clientSockets[i] = clientSocket;
                    std::cout << "Client connected." << std::endl;
                    break;
                }
            }
        }

        // Handle data from clients
        for (int i = 0; i < MAX_CLIENTS; ++i) {

            // Check of incoming connection on CLIENT SOCKET
            if (clientSockets[i] != -1 && FD_ISSET(clientSockets[i], &readfds)) {

                //make char array
                char buffer[1024];

                //sets all values in array to 0
                memset(buffer, 0, sizeof(buffer));

                //in clientSockets[i], recv data (of max size `sizeof(buffer)`,
                //store it in buffer, and there are no special flags (0)
                int bytesRead = recv(clientSockets[i], buffer, sizeof(buffer), 0);

                if (bytesRead <= 0) {
                    // Client disconnected
                    close(clientSockets[i]);
                    std::cout << "Client disconnected." << std::endl;
                    //set value in array as null
                    clientSockets[i] = -1;

                } else {
                    // Calculate Fibonacci
                    int n = std::atoi(buffer);
                    int result = fibonacci(n);
                    std::string fibo = std::to_string(result) + "~\n";
                    send(clientSockets[i], fibo.c_str(), fibo.size(), 0);
                }
            }
        }
    }

    // Close server socket
    close(serverSocket);

    return 0;
}
