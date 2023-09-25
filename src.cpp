#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

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

    // Initialize client sockets to -1 (unused)
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        clientSockets[i] = -1;
    }

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Socket creation failed");
        return 1;
    }

    // Make the server socket non-blocking
    fcntl(serverSocket, F_SETFL, O_NONBLOCK);

    // Initialize server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(PORT);

    // Bind socket to address
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
        FD_SET(serverSocket, &readfds);

        // Add client sockets to the set
        int maxfd = serverSocket;
        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (clientSockets[i] != -1) {
                FD_SET(clientSockets[i], &readfds);
                if (clientSockets[i] > maxfd) {
                    maxfd = clientSockets[i];
                }
            }
        }

        // Wait for activity on sockets
        select(maxfd + 1, &readfds, NULL, NULL, NULL);

        // Check for incoming connection
        if (FD_ISSET(serverSocket, &readfds)) {
            int clientSocket = accept(serverSocket, (struct sockaddr*) &clientAddr, &clientAddrLen);

            // Make the client socket non-blocking
            if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1) {
		perror("Setting socket to non-blocking mode failed");
		close(clientSocket);
		return 1;
	    }

            // Add client socket to the list
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
            if (clientSockets[i] != -1 && FD_ISSET(clientSockets[i], &readfds)) {
                char buffer[1024];
                memset(buffer, 0, sizeof(buffer));

                int bytesRead = recv(clientSockets[i], buffer, sizeof(buffer), 0);

                if (bytesRead <= 0) {
                    // Client disconnected
                    close(clientSockets[i]);
                    std::cout << "Client disconnected." << std::endl;
                    clientSockets[i] = -1;
                } else {
                    // Calculate Fibonacci and send the result back as a string
                    int n = std::atoi(buffer);
                    int result = fibonacci(n);
                    std::string resultStr = std::to_string(result) + "~\n";
                    send(clientSockets[i], resultStr.c_str(), resultStr.size(), 0);
                }
            }
        }
    }

    // Close server socket (this part will never be reached in this example)
    close(serverSocket);

    return 0;
}
