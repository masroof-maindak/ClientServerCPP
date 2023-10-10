#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <random>
#include <vector>       //store image in processing queue
#include <queue>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>    //threads
#include <sys/stat.h>
#include "serverImageProcessing.h"

struct answer {
    int blackBodies;
    unsigned int randomID;
};

struct imageInfo {
    vector<vector<uint8_t>>* img;
    int rows;
    int cols;
    unsigned int randomID;
};

queue<imageInfo> processingQueue;
queue<answer> answerQueue;

//function to handle each client
void* handleClient(void* clientSocketPtr) {
    int clientSocket = *((int*)clientSocketPtr);
    free(clientSocketPtr);

    //receive image rows
    int numRows, numCols;
    ssize_t bytesRead = recv(clientSocket, &numRows, sizeof(numRows), 0);
    if (bytesRead != sizeof(numRows)) {
        perror("Error receiving image rows");
        close(clientSocket); pthread_exit(NULL);
    }

    //and columns
    bytesRead = recv(clientSocket, &numCols, sizeof(numCols), 0);
    if (bytesRead != sizeof(numCols)) {
        perror("Error receiving image columns");
        close(clientSocket); pthread_exit(NULL);
    }

    //receive the image data as binary
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

    //2d matrix to store image
    std::vector<std::vector<uint8_t>> receivedImage(numRows, std::vector<uint8_t>(numCols));
    
    //load the received image data into the matrix
    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j) {
            receivedImage[i][j] = *imageData;
            ++imageData;
        }
    }

    //randomly generated number to serve as identification ID
    std::random_device rd;
    std::mt19937 gen(rd());
    unsigned int randomlyGeneratedID = gen();

    //create data structure to hold processing queue information
    //send vector by reference to prevent unecessary copies
    imageInfo shitToSendToWorker = {&receivedImage, numRows, numCols, randomlyGeneratedID};

    //push to the queue that only the worker thread can pop from
    processingQueue.push(shitToSendToWorker);

    // int charCount = charCounter(receivedImage, numRows, numCols);
    int charCount = -1;

    //pop answer from answer queue now (worker thread pushes its answer to this)
    while(true) {
        if (!answerQueue.empty()) {
            answer temp = answerQueue.front();
            //confirming if element at top of Queue is the one generated for this thread
            if (temp.randomID == randomlyGeneratedID) {
                //if it is, remove it from the answer queue
                answerQueue.pop();
                //store the answer generated for this thread
                charCount = temp.blackBodies;
                //and exit the whileloop
                break;
            }
        }
        //sleep for some time period...?
    }

    //send the number of characters to the client
    if (send(clientSocket, &charCount, sizeof(charCount), 0) == -1) {
        perror("Sending char count failed");
        close(clientSocket); pthread_exit(NULL);
    }

    //Cleanup
    std::cout << "Successfully sent character count." << std::endl;
    // Close the client socket
    close(clientSocket);
    std::cout << "Closing client socket." << std::endl;
    pthread_exit(NULL);
}

int main() {
    //create the worker thread:
    pthread_t workerThread;
    //this thread processes all the images in the processing queue

    //create the thread that handles the answer queue
    pthread_t answerThread; //pops if the top of the queue remains unchanged for a fixed time period
    //e.g when the client disconnects

    //big daddy server socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Socket creation failed"); return 1;
    }

    //make it so that it overwrites a previous iteration's binding to the um uhhh thing
    int reuse = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
        perror("setsockopt"); return 1;
    }

    //bind the socket to an IP address, port and network protocol
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

        pthread_t clientThread;
        if (pthread_create(&clientThread, NULL, handleClient, (void*)clientSocket) != 0) {
            perror("Thread creation failed"); return 1;
        }

        pthread_detach(clientThread); // Detach the thread to allow it to clean up automatically
    }

    // Close sockets
    close(serverSocket);
    return 0;
}
