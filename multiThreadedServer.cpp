#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>
#include <chrono>

// Function to handle each client
void* handleClient(void* clientSocketPtr) {
    int clientSocket = *((int*)clientSocketPtr);
    free(clientSocketPtr); // Free the memory allocated for the client socket pointer

    char buffer[1024];
    std::string receivedFileName;
    std::ofstream outputFile;

    // Receive the filename from the client
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead <= 0) {
        perror("Error receiving filename");
        close(clientSocket);
        pthread_exit(NULL);
    }

    // Extract the filename from the received message
    buffer[bytesRead] = '\0'; // Null-terminate the received data
    std::string receivedMessage(buffer);
    size_t pos = receivedMessage.find("filename:");
    if (pos != std::string::npos) {
        receivedFileName = receivedMessage.substr(pos + 9); // Extract the filename
        std::cout << "Received file name: " << receivedFileName << std::endl;

        // Open the file for writing
        outputFile.open(receivedFileName, std::ios::out);
        if (!outputFile.is_open()) {
            perror("Error opening output file");
            close(clientSocket);
            pthread_exit(NULL);
        }
    } else {
        std::cerr << "Invalid message format. Expected 'filename: <filename>'." << std::endl;
        close(clientSocket);
        pthread_exit(NULL);
    }

    // Receive and write the file content line by line
    while (true) {
        bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            break; // End of file or error
        }
        outputFile.write(buffer, bytesRead);
    }

    // Close the output file and client socket
    outputFile.close();
    close(clientSocket);

    std::cout << "Received file '" << receivedFileName << "' successfully!" << std::endl;

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