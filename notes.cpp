#include <iostream>
#include <cstring>       // for memset
#include <unistd.h>      // for close
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>       //non-blocking 
#include <vector>
#include <algorithm>
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


int main() {
    int clientSocket, serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    //af_inet = address family (ipv4) - others include AF_INET6 (ipv6), AF_UNIX (IPC comm. on the same device)
    //sock_stream = socket type (in this case, STREAM). Stream sockets allow for connection-oriented communication. Others include SOCK_RAW, SOCK_SEQPACKET, etc.
    //0 = protocol (default - OS selects based on address family and socket type)
    
    //if socket doesn't get created, program exits
    if (serverSocket == -1) {
        perror("Socket creation failed");
        return 1;
    }

    // Bind the socket to an IP address and port
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    serverAddr.sin_family = AF_INET;	//sin = socket internet
					//yes, it's fucking stupid.
    serverAddr.sin_port = htons(8080);  // Port 8080
    serverAddr.sin_addr.s_addr = INADDR_ANY; // All available network interfaces
					     // sin_addr - the field used to specify the IP address associated with the socket. Only sort of a wrapper for the s_addr value in this case. Exists for compatibility, extensibility, readability, and consistency. Keeps API versatile and accomodates future changes.
					     //s_addr is used to store the actual IP address in binary format.

    //bind - associates socket with specific network address (IP + port)
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
    std::vector<int> clientSockets;

    // Communication with the client
    while (true) {
	    // Accept incoming connections (non-blocking)
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen);

        if (clientSocket == -1) {
            // No incoming connections, continue with the loop
            usleep(100000); // Sleep briefly to avoid busy-waiting
            continue;
        }

        // Set the client socket to non-blocking mode
        if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1) {
            perror("Setting client socket to non-blocking mode failed");
            close(clientSocket);
            continue;
        }

        // Handle the client socket
        int n;
        ssize_t bytesRead = recv(clientSocket, &n, sizeof(int), 0);

        if (bytesRead == sizeof(int)) {
            int fibo = fibonacci(n);
            std::string nthFibo = std::to_string(fibo) + "~\n";
            send(clientSocket, nthFibo.c_str(), nthFibo.length(), 0);
        }

        // Close the client socket
        close(clientSocket);

	// Remove the client socket from the vector
	auto it = std::remove(clientSockets.begin(), clientSockets.end(), clientSocket);
    	clientSockets.erase(it, clientSockets.end());	
    }


    //close client sockets
    for (int clientSock : clientSockets) {
        close(clientSock);
    }

    // Close sockets
    close(serverSocket);

    return 0;
}
