#include <iostream>      //not sure about this one chief

#include <unistd.h>      //socket-handling + system-level operations
#include <sys/socket.h>  //for sockets

#include <netinet/in.h>  //network structures, e.g sockaddr_in, used in conjunction with;
#include <arpa/inet.h>   //handling IP addresses + converting b/w host & network addresses
#include <pthread.h>     //threads

#include <random>        //thread image ID generation
#include <vector>        //store image
#include <queue>         //processing/answer queues
#include <chrono>        //answer queue pops un-picked-up answer

#include "serverImage.h" //flood fill algo to count characters in the image

#define SLEEP_TIME 400000 //0.4s

//structs
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

//global queues
queue<imageInfo> processingQueue;
queue<answer> answerQueue;

//client thread
void* receiveImage(void* clientSocketPtr) {
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

    //2d matrix to store image
    std::vector<std::vector<uint8_t>> receivedImage(numRows, std::vector<uint8_t>(numCols));

    //receive image row by row
    for (int row = 0; row < numRows; row++) {
        int intSize = sizeof(uint32_t);
        int arraySize = numCols + intSize;

        //make array to recieve rowID + image row
        uint8_t rowData[arraySize];

        //recieves rowID + row into rowData
        bytesRead = recv(clientSocket, rowData, arraySize, 0);
        if (bytesRead < 1) {
            perror("Error receiving image data");
            close(clientSocket);
            pthread_exit(NULL);
        }

        //store row as uint32_t (int) pointer
        //reinterpret cast force-converts it
        uint32_t rowID = *reinterpret_cast<uint32_t*>(rowData);

        //rowImageData now points to where image starts from
        uint8_t* rowImageData = rowData + intSize;

        //copy contents of rowImageData short[] heap array over to vector matrix
        for(int col = 0; col < numCols; col++) {
            receivedImage[row][col] = rowImageData[col];
        }
    }

    //RECV() arguments:
    //1. socket from where I want to RECEIVE data
    //2. Pointer to a buffer variable where I'll store said data
    //3. size of buffer in bytes
    //4. int flags, 0 - none

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
        } else {
            usleep(SLEEP_TIME); 
        }
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

//worker thread
void* readProcessingQueue(void* arg) {
    while (true) {
        if (!processingQueue.empty()) {
            imageInfo tempImg = processingQueue.front();
            processingQueue.pop();
            int ans = charCounter (*tempImg.img, tempImg.rows, tempImg.cols);
            answer tempAns = {ans, tempImg.randomID};
            answerQueue.push(tempAns);
        } else {
            usleep(SLEEP_TIME);
        }
    }
    return NULL;
}

//answer thread
void* readAnswerQueue(void* arg) {
    std::chrono::seconds timeout(5);

    //forever
    while (true) {
        //if answerQ not emprty
        if (!answerQueue.empty()) {
            //current answer = front of queue
            int currAns = answerQueue.front().blackBodies;
            int prevValue = currAns;

            if (answerQueue.size() == 1 || currAns != prevValue) {
                prevValue = currAns;
                auto startTime = std::chrono::steady_clock::now();
                while (true) {
                    //check elapsed time
                    auto elapsed_time = std::chrono::steady_clock::now() - startTime;
                    //if it exceeds our time out value
                    if (elapsed_time >= timeout) {
                        //leave inner while
                        break;
                    }

                    // Sleep for a short duration to avoid busy-waitings
                    struct timespec sleepTime;
                    sleepTime.tv_sec = 0;
                    sleepTime.tv_nsec = 100000000; //100 ms
                    nanosleep(&sleepTime, NULL);
                }
            }

            if (currAns == answerQueue.front().blackBodies) {
                //value at queue front still matches previously recorded; pop it.
                answerQueue.pop();
                std::cout << "Popped value: " << currAns << "; Client might have disconnected" << std::endl;
            }
        } else {
            usleep(SLEEP_TIME);
        }
    }
    return NULL;
}

int main() {
    //create the worker thread
    //this thread processes all the images in the processing queue
    pthread_t workerThread;
    if (pthread_create(&workerThread, NULL, readProcessingQueue, NULL) != 0) {
        perror("Worker thread creation failed"); return 1;
    }

    //create the thread that handles the answer queue
    pthread_t answerThread; //pops if the top of the queue remains unchanged for a fixed time period
    //e.g when the client disconnects
    if (pthread_create(&answerThread, NULL, readAnswerQueue, NULL) != 0) {
        perror("Answer thread creation failed"); return 1;
    }

    //PTHREAD_CREATE() arguments:
    //1. thread by reference
    //2. thread attributes, e.g stack size/scheduling policy
    //3. function it's working in (scope of reference?)
    //4. function parameters

    //big daddy server socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Socket creation failed"); return 1;
    }

    //SOCKET() arguments:
    //af_inet = address family (ipv4) - others include AF_INET6 (ipv6), AF_UNIX (IPC comm. on the same device)
    //sock_stream = socket type (in this case, STREAM). Stream sockets allow for connection-oriented communication. Others include SOCK_RAW, SOCK_SEQPACKET, etc.
    //0 = protocol (default - OS selects based on address family and socket type)

    //make it so that it overwrites a previous iteration's binding to the um uhhh thing
    int reuse = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
        perror("setsockopt"); return 1;
    }

    //bind the socket to an IP address, port and network protocol
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;    //sin = socket internet
                    					//yes, it's fucking stupid.
    serverAddr.sin_port = htons(8080);  //port 8080 basically - htons = host TO network short (like the data type)
    serverAddr.sin_addr.s_addr = INADDR_ANY; //// All available network interfaces - e.g Wifi/Ethernet/Bluetooth, etc.
    // sin_addr - the field used to specify the IP address associated with the socket. 
    // Only sort of a wrapper for the s_addr value in this case. 
    // Exists for compatibility, extensibility, readability, and consistency. 
    // Keeps API versatile and accomodates future changes.
	// s_addr is used to store the actual IP address in binary format.

    //bind - associates socket with specific network address (IP + port)
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Binding failed"); return 1;
    }

    //isten for incoming connections
    if (listen(serverSocket, 5) == -1) {
        perror("Listening failed"); return 1;
    }

    //LISTEN() arguments:
    //1. make this socket ready to accept connections
    //2. accept, at most, this value, before refusing all further connections

    //public service announcement
    std::cout << "Server listening on port 8080..." << std::endl;

    //connecting with client(s) (infinitely)
    while (true) {
        //struct to store client info
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        //dynamically allocated memory to store int (i.e socket file descriptor) for incoming client
        int* clientSocket = (int*)malloc(sizeof(int));
        //'accept' function. Blocking until client connects.
        *clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);

        //ACCEPT() arguments:
        //1. int socket fd, this is the socket that is waiting for clients to connect to it
        //2. a pointer to the sockAddr that will be filled with the client's information when it connects
        //   usually cast to (struct sockaddr*), but can also be cast to specific socket address structure 
        //   like struct sockaddr_in for IPv4 or struct sockaddr_in6 for IPv6.
        //3. Pointer to socklen_t variable that stores size of addr.

        //in case of error, accept() returns -1
        if (*clientSocket == -1) {
            perror("Failed to accept connection!"); return 1;
        }

        //create client thread
        //inits the client in a separate function, passing the socket as a parameter
        pthread_t clientThread;
        if (pthread_create(&clientThread, NULL, receiveImage, (void*)clientSocket) != 0) {
            perror("Thread creation failed"); return 1;
        }

        pthread_detach(clientThread); // Detach the thread to allow it to clean up automatically
    }

    // Close sockets
    close(serverSocket);
    return 0;
}
