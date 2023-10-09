#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include "image/Image.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <image_file>" << std::endl;
        return 1;
    }

    // Read the image file name from the command line argument
    const char* imageFileName = argv[1];

    // Load the original image using your Image class
    Image originalImage(imageFileName);

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

    // Send the original image dimensions (rows and columns) to the server
    int numRows = originalImage.get_rows();
    int numCols = originalImage.get_cols();
    if (send(clientSocket, &numRows, sizeof(numRows), 0) == -1) {
        perror("Sending original image rows failed");
        close(clientSocket);
        exit(EXIT_FAILURE);
    }

    if (send(clientSocket, &numCols, sizeof(numCols), 0) == -1) {
        perror("Sending original image columns failed");
        close(clientSocket);
        exit(EXIT_FAILURE);
    }

    // Send the original image data as binary
    ssize_t imageSize = numRows * numCols * sizeof(uint8_t);
    if (send(clientSocket, originalImage[0], imageSize, 0) == -1) {
        perror("Sending original image data failed");
        close(clientSocket);
        exit(EXIT_FAILURE);
    }

    // Receive the required information from the server
    int numOfCharacters;
    ssize_t bytesRead = recv(clientSocket, &numOfCharacters, sizeof(numOfCharacters), 0);
    if (bytesRead != sizeof(numOfCharacters)) {
        perror("Error receiving character count");
        close(clientSocket);
        exit(EXIT_FAILURE);
    }

    std::cout << "Number of characters: " << numOfCharacters << std::endl;

    // Close the client socket
    close(clientSocket);

    return 0;
}