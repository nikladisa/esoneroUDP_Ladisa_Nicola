// Conditional inclusion based on the operating system
#if defined WIN32
#include <winsock2.h>
#else
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#endif

#include <stdio.h>
#include <ctype.h>
#include "math_functions.h"

// Function to release Winsock resources on Windows
void clearwinsock() {
#if defined WIN32
    WSACleanup();
#endif
}

// Function to handle error messages
void errorhandler(char *errorMessage) {
    printf("%s", errorMessage);
}

int main() {
#if defined WIN32
    // Initialize Winsock for the Win32 platform
    WSADATA wsa_data;
    int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (result != NO_ERROR) {
        printf("Error in WSAStartup()\n");
        return 0;
    }
#endif

    // Retrieve server information
    ServerStat serverstats = getServer();
    char *ip = "";
    struct hostent *host;
    host = gethostbyname(serverstats.server);
    if (host == NULL) {
        fprintf(stderr, "gethostbyname() failed");
    }

    // Extract IP address from host information
    struct in_addr ina = *(struct in_addr *)host->h_addr_list[0];
    ip = inet_ntoa(ina);

    // Server address structure
    struct sockaddr_in server_addr;

    // Client socket address structure
    struct sockaddr_in FromADDR;
    int fromSize;
    char buffer[500];
    int respStringl;

    // Create a socket for the client
    int cSocket = socket(AF_INET, SOCK_DGRAM, 0); // Use SOCK_DGRAM for UDP
    if (cSocket < 0) {
        errorhandler("Error creating socket\n");
        clearwinsock();
        return 1;
    } else {
        printf("Socket created\n");
    }

    // Configure the server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(serverstats.port);

    char input[500];
    char operator;
    int num1;
    int num2;

    // Main client loop
    while (1) {
        printf("Insert operation, number one, and number two. [operation space number one space number two]\n");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        // Check if the operator is '=' and exit the loop if true
        if (input[0] == '=' && input[1] == '\n') {
            printf("Closing the system, you entered '='\n");
            break;
        }

        // Parse user input for operator and numbers
        if (sscanf(input, "%c %d %d", &operator, &num1, &num2) != 3) {
            errorhandler("Input not valid\n");
            continue;
        }

        // Check for a space after the operator
        if (input[1] != ' ') {
            errorhandler("Insert a space after the operator\n");
            continue;
        }

        // Check if the operator is valid
        if (operator != '+' && operator != '-' && operator != 'x' && operator != '/') {
            errorhandler("Invalid operator\n");
            continue;
        }

        // Send operator and numbers to the server
        sendto(cSocket, input, strlen(input), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

        // Receive the result from the server
        fromSize = sizeof(FromADDR);
        respStringl = recvfrom(cSocket, buffer, 500, 0, (struct sockaddr *)&FromADDR, &fromSize);

        // Check if the response is from the expected server
        if (server_addr.sin_addr.s_addr != FromADDR.sin_addr.s_addr) {
            errorhandler("Error: unknown package\n");
            break;
        }

        // Null-terminate the received string
        buffer[respStringl] = '\0';

        // Print the received result from the server
        printf("Received result from server %s, IP %d: %s\n", serverstats.server, serverstats.port, buffer);
    }

    // Close the client socket and release Winsock resources
    if (closesocket(cSocket) == SOCKET_ERROR) {
        errorhandler("Error closing the client socket\n");
    }

    clearwinsock();
    return 0;
}
