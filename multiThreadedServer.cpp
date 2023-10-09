#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <vector>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>    //threads
#include <sys/stat.h>
#include "serverImageProcessing.h"

//function to handle each client
void* handleClient(void* clientSocketPtr) {
    int clientSocket = *((int*)clientSocketPtr);
    free(clientSocketPtr);

    //receive image dimensions (rows and columns)
    int numRows, numCols;
    ssize_t bytesRead = recv(clientSocket, &numRows, sizeof(numRows), 0);
    if (bytesRead != sizeof(numRows)) {
        perror("Error receiving image rows");
        close(clientSocket); pthread_exit(NULL);
    }

    bytesRead = recv(clientSocket, &numCols, sizeof(numCols), 0);
    if (bytesRead != sizeof(numCols)) {
        perror("Error receiving image columns");
        close(clientSocket); pthread_exit(NULL);
    }

    // Receive the image data as binary
    ssize_t imageSize = numRows * numCols * sizeof(uint8_t);
    uint8_t* imageData = new uint8_t[imageSize];
    uint8_t* imageDataPtr = imageData;

    while (imageSize > 0) {
        bytesRead = recv(clientSocket, imageDataPtr, imageSize, 0);
        if (bytesRead <= 0) {
            perror("Error receiving image data");
            close(clientSocket);
            delete[] imageData;
            pthread_exit(NULL);
        }
        imageSize -= bytesRead;
        imageDataPtr += bytesRead;
    }

    // Create a 2D matrix to store the image data
    std::vector<std::vector<uint8_t>> receivedImage(numRows, std::vector<uint8_t>(numCols));
    
    // Load the received image data into the matrix
    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j) {
            receivedImage[i][j] = *imageData;
            ++imageData;
        }
    }

    int charCount = charCounter(receivedImage, numRows, numCols);

    // Send the updated matrix dimensions (rows and columns) to the client
    if (send(clientSocket, &charCount, sizeof(charCount), 0) == -1) {
        perror("Sending char count failed");
        close(clientSocket); pthread_exit(NULL);
    }
    
    std::cout << "Successfully sent character count." << std::endl;

    // Close the client socket
    close(clientSocket);

    std::cout << "Closing client socket." << std::endl;

    pthread_exit(NULL);
}

int main() {
    // Create a socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Socket creation failed"); return 1;
    }

    int reuse = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
        perror("setsockopt"); return 1;
    }

    // Bind the socket to an IP address and port
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Binding failed"); return 1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, 5) == -1) {
        perror("Listening failed"); return 1;
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
            perror("Failed to accept connection!"); return 1;
        }

        pthread_t thread;
        if (pthread_create(&thread, NULL, handleClient, (void*)clientSocket) != 0) {
            perror("Thread creation failed"); return 1;
        }

        pthread_detach(thread); // Detach the thread to allow it to clean up automatically
    }

    // Close sockets
    close(serverSocket);
    return 0;
}
