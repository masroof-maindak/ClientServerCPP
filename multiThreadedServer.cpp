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

void receiveFile(int clientSocket, const char* filePath, off_t fileSize) {
    int fileDescriptor = open(filePath, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fileDescriptor == -1) {
        perror("Failed to open file for writing");
        return;
    }

    char buffer[1024];
    ssize_t bytesRead;
    off_t bytesReceived = 0;

    while (bytesReceived < fileSize) {
        bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead == -1) {
            // Handle non-blocking operation (e.g., no data available)
            // You can add custom logic here if needed
            usleep(10000); // Sleep for 10 milliseconds (adjust as needed)
            continue;
        } else if (bytesRead == 0) {
            // Client disconnected prematurely
            std::cerr << "Client disconnected prematurely" << std::endl;
            close(fileDescriptor);
            return;
        }

        if (write(fileDescriptor, buffer, bytesRead) == -1) {
            perror("Failed to write to file");
            close(fileDescriptor);
            return;
        }

        bytesReceived += bytesRead;
    }

    close(fileDescriptor);
}

void* handleClient(void* arg) {
    int clientSocket = *((int*)arg);
    free(arg);

    // Set the client socket to non-blocking mode
    if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1) {
        perror("Setting socket to non-blocking mode failed");
        close(clientSocket);
        pthread_exit(NULL);
    }

    auto now = std::chrono::system_clock::now();
    std::time_t timeNow = std::chrono::system_clock::to_time_t(now);
    std::cout << "Client connected at " << std::ctime(&timeNow) << std::endl;

    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    ssize_t bytesRead;

    // Initialize variables for file reception
    std::string fileName;
    off_t fileSize = -1;
    bool receivingFile = false;
    int fileDescriptor = -1;

    // Loop to receive and process requests from the client
    while (true) {
        bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesRead < 0) {
            // Handle non-blocking operation (e.g., no data available)
            // You can add custom logic here if needed
        } else if (bytesRead == 0) {
            // Client disconnected
            now = std::chrono::system_clock::now();
            timeNow = std::chrono::system_clock::to_time_t(now);
            std::cout << "Client disconnected at " << std::ctime(&timeNow) << std::endl;
            break;
        } else {
            if (!receivingFile) {
                // Check for the "filename:" field in the received data
                std::string data(buffer, bytesRead);
                size_t filenamePos = data.find("filename:");
                if (filenamePos != std::string::npos) {
                    // Extract the file name
                    size_t filenameEnd = data.find_first_of("\n", filenamePos);
                    if (filenameEnd != std::string::npos) {
                        fileName = data.substr(filenamePos + 9, filenameEnd - filenamePos - 9);
                        fileSize = bytesRead - filenameEnd - 1;
                        receivingFile = true;

                        // Open the file for writing
                        fileDescriptor = open(fileName.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
                        if (fileDescriptor == -1) {
                            perror("Failed to open file for writing");
                            close(clientSocket);
                            pthread_exit(NULL);
                        }

                        // Write the remaining data (file content) to the file
                        if (write(fileDescriptor, data.c_str() + filenameEnd + 1, fileSize) == -1) {
                            perror("Failed to write to file");
                            close(fileDescriptor);
                            close(clientSocket);
                            pthread_exit(NULL);
                        }

                        // Update the remaining file size
                        fileSize -= bytesRead - filenameEnd - 1;
                    }
                }
            } else {
                // Continue writing received data to the file
                if (write(fileDescriptor, buffer, bytesRead) == -1) {
                    perror("Failed to write to file");
                    close(fileDescriptor);
                    close(clientSocket);
                    pthread_exit(NULL);
                }

                // Update the remaining file size
                fileSize -= bytesRead;

                // Check if the entire file has been received
                if (fileSize <= 0) {
                    close(fileDescriptor);
                    receivingFile = false;
                    fileSize = -1;
                    fileName.clear();
                    std::cout << "File received: " << fileName << std::endl;
                    send(clientSocket, "File received\n", 14, 0);
                }
            }
        }

        // Clear the buffer for the next iteration
        memset(buffer, 0, sizeof(buffer));
    }

    close(clientSocket);
    pthread_exit(NULL);
}

//SHOULD BE GOOD CHECK REST!
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