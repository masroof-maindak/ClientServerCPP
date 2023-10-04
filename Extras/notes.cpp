#include <cstring>       // for memset
#include <unistd.h>      // for close
#include <fcntl.h>       // non-blocking 

int main() {
    /*

    int clientSocket, serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    
    //af_inet = address family (ipv4) - others include AF_INET6 (ipv6), AF_UNIX (IPC comm. on the same device)
    //sock_stream = socket type (in this case, STREAM). Stream sockets allow for connection-oriented communication. Others include SOCK_RAW, SOCK_SEQPACKET, etc.
    //0 = protocol (default - OS selects based on address family and socket type)
    
    // Bind the socket to an IP address and port
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    serverAddr.sin_family = AF_INET;	//sin = socket internet
					//yes, it's fucking stupid.
    serverAddr.sin_port = htons(8080);  // Port 8080
    serverAddr.sin_addr.s_addr = INADDR_ANY; // All available network interfaces
					     // sin_addr - the field used to specify the IP address associated with the socket. Only sort of a wrapper for the s_addr value in this case. Exists for compatibility, extensibility, readability, and consistency. Keeps API versatile and accomodates future changes.
					     //s_addr is used to store the actual IP address in binary format.
					     // sin_addr - the field used to specify the IP address associated with the socket. 
                         // Only sort of a wrapper for the s_addr value in this case. 
                         // Exists for compatibility, extensibility, readability, and consistency. 
                         // Keeps API versatile and accomodates future changes.
					     // s_addr is used to store the actual IP address in binary format.

    //bind - associates socket with specific network address (IP + port)
    
    
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Binding failed");
        return 1;
    }
    return 0;

    */
}
