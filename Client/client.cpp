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
    //help message
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <image_file>" << std::endl;
        return 1;
    }

    //store file name and open image
    const char* imageFileName = argv[1];
    Image originalImage(imageFileName);

    //init client socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    //bind it
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET; //ipv4
    serverAddr.sin_port = htons(8080); //port 8080

    //convert human-readable string to binary network address structure
    if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) < 1) {
        perror("Invalid address or address not supported");
        exit(EXIT_FAILURE);
    }

    //INET_PTON() arguments:
    //1. address family (ipv4/v6)
    //2. pointer to the ip address string you want to convert
    //3. pointer to the locatoin where you want to store the converted binary address

    //connect client to server via sockets
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    //CONNECT() arguments:
    //1. socket file descriptor of client socket you want to connect
    //2. pointer to sockaddr struct holding info. Cast exists to convert to ipv4/v6
    //3. size of addr structure

    int numRows = originalImage.get_rows();
    int numCols = originalImage.get_cols();

    //send row number
    if (send(clientSocket, &numRows, sizeof(numRows), 0) == -1) {
        perror("Sending original image rows failed");
        close(clientSocket); exit(EXIT_FAILURE);
    }

    //send column number
    if (send(clientSocket, &numCols, sizeof(numCols), 0) == -1) {
        perror("Sending original image columns failed");
        close(clientSocket); exit(EXIT_FAILURE);
    }

    //transfer image row by row
    for (int row = 0; row < numRows; row++) {
        int intSize = sizeof(uint32_t);
        int arraySize = numCols + intSize;

        //make array to embed row id in image
        uint8_t rowData[arraySize];

        //put row id in first 4 indices
        for(int i = 0; i < intSize; i++) {
            rowData[i] = ((char *)&row)[i];
        }

        //copy over the rest of the elements
        for(int i = 0; i < numCols; i++) {
            rowData[i + intSize] = originalImage[row][i];
        }

        //send row
        if (send(clientSocket, rowData, arraySize, 0) == -1) {
            perror("Sending image row failed");
            close(clientSocket); exit(EXIT_FAILURE);
        }

        //acknowledgement byte
        while(true) {
            int acknowledgement;
            if (recv(clientSocket, &acknowledgement, intSize, 0)) == -1) {
                perror("Receiving acknowledgement failed.");
                close(clientSocket); pthread_exit(NULL);
            }
            if(acknowledgement == 1) {break;} 
            else {usleep(40000);}
        }
    }


    //recv answer from server
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
