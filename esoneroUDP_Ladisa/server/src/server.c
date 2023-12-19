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

// Function to clean up Winsock resources if applicable
void clearwinsock() {
#if defined WIN32
    WSACleanup();
#endif
}

// Function to handle errors and print error messages
void errorhandler(char *errorMessage) {
    printf("%s", errorMessage);
}

int main(int argc, char *argv[]) {
#if defined WIN32
    // Initialize Winsock for Windows
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
    struct in_addr ina = *(struct in_addr *)host->h_addr_list[0];
    ip = inet_ntoa(ina);

    // Set up server and client socket structures
    struct sockaddr_in server_addr, client_addr;
    int client_addr_len = sizeof(client_addr);
    int sSocket = socket(AF_INET, SOCK_DGRAM, 0); // Use SOCK_DGRAM for UDP
    if (sSocket < 0) {
        errorhandler("Error creating socket\n");
        clearwinsock();
        return 1;
    } else {
        printf("Socket created\n");
    }

    // Set up server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(serverstats.port);

    // Bind the socket
    if (bind(sSocket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        errorhandler("bind() failed.\n");
        system("pause");
        closesocket(sSocket);
        clearwinsock();
        return 1;
    }

    char operat;
    int connum1;
    int connum2;

    // Main server loop
    while (1) {
        printf("\nWaiting for data...");

        char input[500];
        char output[500];

        // Receive data from the client
        int byterec = recvfrom(sSocket, input, sizeof(input), 0, (struct sockaddr *)&client_addr, &client_addr_len);

        // Parse the received message to extract operator and numbers
        if (sscanf(input, "%c %d %d", &operat, &connum1, &connum2) != 3) {
            errorhandler("Invalid input from the client\n");
            continue;
        }

        // Get client information
        host = gethostbyaddr((char *)&client_addr.sin_addr, 4, AF_INET);
        char *clientN = host->h_name;
        printf("Request operation %c %d %d from client %s, IP %s\n", operat, connum1, connum2, clientN, inet_ntoa(client_addr.sin_addr));

        int risult;

        // Perform the requested operation
        switch (operat) {
            case '+':
                risult = add(connum1, connum2);
                break;
            case '-':
                risult = sub(connum1, connum2);
                break;
            case 'x':
                risult = mult(connum1, connum2);
                break;
            case '/':
                risult = division(connum1, connum2);
                break;
            default:
                errorhandler("Invalid operator\n");
                continue;
        }

        // Format the output message
        sprintf(output, "%d %c %d = %d", connum1, operat, connum2, risult);
        int len = strlen(output);

        // Send the result back to the client
        sendto(sSocket, output, len, 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
    }

    // Close the socket and clean up resources
    closesocket(sSocket);
    clearwinsock();

    return 0;
}

