#include <iostream>
#include <cstring>       // for memset
#include <unistd.h>      // for close
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>       //non-blocking 
#include <thread>
#include <vector>
#include "Queue/queue.h"

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

void handleClient(int clientSocket) {

    // Set the client socket to non-blocking mode
    if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1) {
        perror("Setting socket to non-blocking mode failed");
        close(clientSocket);
        return;
    }

    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    ssize_t bytesRead;

    // Loop to receive and process data from the client
    while (true) {
        bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesRead < 0) {
            // Handle non-blocking operation (e.g., no data available)
            // You can add custom logic here if needed
        } else if (bytesRead == 0) {
            // Client disconnected
            std::cout << "Client disconnected" << std::endl;
            break;
        } else {
            int n = std::stoi(buffer);
            int fibo = fibonacci(n);
            std::string nthFibo = std::to_string(fibo) + "~\n";
            // Process and respond to the received data
            // Example: send back the received data
            send(clientSocket, nthFibo.c_str(), nthFibo.length(), 0);
        }

        // Clear the buffer for the next iteration
        memset(buffer, 0, sizeof(buffer));
    }



    close(clientSocket);
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

    // store new client in a thread
    std::vector<std::thread> clientThreads;



    // Communication with the client
    while (true) {
        
        // Accept incoming connections
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket == -1) {
            perror("Failed to accept connection!");
            return 1;
        }

        std::cout << "Client connected" << std::endl;

        clientThreads.emplace_back(handleClient, clientSocket);

    }

    // Close sockets
    close(serverSocket);

    return 0;
}
