#include <iostream>
#include <cstring>       // for memset
#include <unistd.h>      // for close
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>       //non-blocking 
#include <pthread.h>     // for pthread
#include <chrono>
#include <ctime>
#include "Queue/queue.h"

int fib(int n) {
    if (n < 2) return n;
    return fib(n - 1) + fib(n - 2);
}

void* handleClient(void* arg) {
    //cast arg to int and dereference it
    int clientSocket = *((int*)arg);
    free(arg);

    // Set the client socket to non-blocking mode
    if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1) {
        perror("Setting socket to non-blocking mode failed");
        close(clientSocket);
        pthread_exit(NULL);
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
            int fibo = fib(n);
            std::string nthFibo = std::to_string(fibo) + "~\n";
            // Process and respond to the received data
            // Example: send back the received data
            send(clientSocket, nthFibo.c_str(), nthFibo.length(), 0);
        }

        // Clear the buffer for the next iteration
        memset(buffer, 0, sizeof(buffer));
    }

    close(clientSocket);
    pthread_exit(NULL);
}

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
    if (listen(serverSocket, 5) == -1) {
        perror("Listening failed");
        return 1;
    }

    std::cout << "Server listening on port 8080..." << std::endl;

    // Communication with the client
    while (true) {
        // Accept incoming connections
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int* clientSocket = (int*)malloc(sizeof(int)); // Allocate memory for the socket
        *clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (*clientSocket == -1) {
            perror("Failed to accept connection!");
            return 1;
        }

        auto now = std::chrono::system_clock::now(); 
        std::time_t timeNow = std::chrono::system_clock::to_time_t(now);
        std::cout << "Client connected at " << std::ctime(&timeNow) << std::endl;

        pthread_t thread;
        if (pthread_create(&thread, NULL, handleClient, (void*)clientSocket) != 0) {
            perror("Thread creation failed");
            return 1;
        }
        pthread_detach(thread); // Detach the thread to allow it to clean up automatically
    }

    // Close sockets
    close(serverSocket);

    return 0;
}