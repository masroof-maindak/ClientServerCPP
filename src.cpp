#include <iostream>
#include <cstring>       // for memset
#include <unistd.h>      // for close
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

int main() {
    // Create a socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Socket creation failed");
        return 1;
    }

    // Bind the socket to an IP address and port
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);  // Replace with your desired port
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Binding failed");
        return 1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, 5) == -1) {  // 5 is the maximum number of pending connections
        perror("Listening failed");
        return 1;
    }

    std::cout << "Server listening on port 8080..." << std::endl;

    // Accept incoming connections
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (clientSocket == -1) {
        perror("Accepting connection failed");
        return 1;
    }

    std::cout << "Client connected" << std::endl;

    // Communication with the client
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            if (bytesRead == 0) {
                std::cout << "Client disconnected" << std::endl;
            } else {
                perror("Error in receiving data");
            }
            break;
        }

        // Process and respond to the received data
        // Example: send back the received data
        send(clientSocket, buffer, bytesRead, 0);
    }

    // Close sockets
    close(clientSocket);
    close(serverSocket);

    return 0;
}
