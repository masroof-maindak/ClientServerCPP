#include <iostream>
#include <cstring>       // for memset
#include <unistd.h>      // for close
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

int fibonacci(int n) {
    if (n <= 0) {
        return 0;
    } else if (n == 1) {
        return 1;
    } else {
        return fibonacci(n - 1) + fibonacci(n - 2);
    }
}

int main() {
    // Create a socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    //af_inet = address family (ipv4)
    //sock_stream = socket type (tcp)
    //0 = protocol (default)
    
    //if socket doesn't get created, program exits
    if (serverSocket == -1) {
        perror("Socket creation failed");
        return 1;
    }

    // Bind the socket to an IP address and port
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);  // Replace with your desired port
    serverAddr.sin_addr.s_addr = INADDR_ANY; // All available network interfaces

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
        perror("Failed to accept connection!");
        return 1;
    }

    std::cout << "Client connected" << std::endl;

    // Communication with the client
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        //check if client transmitted 0 bytes (disconnected)
        if (bytesRead <= 0) {
            if (bytesRead == 0) {
                std::cout << "Client disconnected" << std::endl;
            } else {
                perror("Error in receiving data");
            }
            break;
        }

        //get nth fibonacci number from user input
        int n = std::stoi(buffer);
        int fibo = fibonacci(n);
        std::string nthFibo = std::to_string(fibo) + "~\n";

        // Process and respond to the received data
        // Example: send back the received data
        send(clientSocket, nthFibo.c_str(), nthFibo.length(), 0);
    }

    // Close sockets
    close(clientSocket);
    close(serverSocket);

    return 0;
}
