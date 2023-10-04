#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fstream>
#include <string>

int main() {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080); // Port 8080

    if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) <= 0) {
        perror("Invalid address or address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    // Prompt the user for the file path to send
    char filePath[256];
    std::cout << "Enter the file path to send: ";
    std::cin.getline(filePath, sizeof(filePath));

    // Check if the file exists and can be opened
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open the file: " << filePath << std::endl;
        close(clientSocket);
        exit(EXIT_FAILURE);
    }

    // Send the file name as part of the request
    std::string fileName = "filename:" + std::string(filePath);
    if (send(clientSocket, fileName.c_str(), fileName.length(), 0) == -1) {
        perror("Sending file name failed");
        file.close();  // Close the file before exiting
        close(clientSocket);
        exit(EXIT_FAILURE);
    }

    // Send the file content line by line
    std::string line;
    while (std::getline(file, line)) {
        line += '\n'; // Add the newline character
        if (send(clientSocket, line.c_str(), line.length(), 0) == -1) {
            perror("Sending normal line failed");
            file.close();  // Close the file before exiting
            close(clientSocket);
            exit(EXIT_FAILURE);
        }
    }

    std::cout << "File sent successfully!" << std::endl;

    // Communication with the server
    char buffer[1024];
    while (true) {
        // Receive a response from the server
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead == -1) {
            perror("Receive failed");
            file.close();  // Close the file before exiting
            close(clientSocket);
            exit(EXIT_FAILURE);
        } else if (bytesRead == 0) {
            // Server disconnected
            std::cout << "Server disconnected" << std::endl;
            break;
        }

        std::cout << "Server response: " << buffer << std::endl;
    }

    // Close the file and client socket before exiting
    file.close();
    close(clientSocket);
    return 0;
}