// Include guard to prevent multiple inclusion of the header file
#ifndef MATH_FUNCTIONS_H
#define MATH_FUNCTIONS_H

// Default server address and port if not provided by the user
#define standard_addres "127.0.0.1"
#define standard_port 56700

// Function for addition
int add(int a, int b) {
    return a + b;
}

// Function for multiplication
int mult(int a, int b) {
    return a * b;
}

// Function for subtraction
int sub(int a, int b) {
    return a - b;
}

// Function for division
int division(int a, int b) {
    if (b != 0) {
        return a / b;
    } else {
        return 0;
    }
}

// Structure to store server information
typedef struct {
    char server[500];
    int port;
} ServerStat;

// Function to get server information from the user
ServerStat getServer() {
    ServerStat info;

    // Buffer to store user input for server information
    char inputServ[500];

    // Prompt the user to enter hostname:port (format: hostname:port)
    printf("Enter hostname:port (format: hostname:port) [empty for default %s:%d]\n", standard_addres, standard_port);

    // Read user input
    fgets(inputServ, sizeof(inputServ), stdin);

    // Remove newline character if present in the input
    size_t len = strlen(inputServ);
    if (len > 0 && inputServ[len - 1] == '\n') {
        inputServ[len - 1] = '\0';
    }

    // Use default values if user input is empty
    if (strlen(inputServ) == 0) {
        sprintf(inputServ, "%s:%d", standard_addres, standard_port);
    }

    // Validate input format (hostname:port)
    if (!strchr(inputServ, ':') != NULL) {
        printf("Insert a valid format: hostname:port");
        return getServer();
    }

    // Parse input to extract server and port
    sscanf(inputServ, "%[^:]:%d", info.server, &info.port);

    // Validate port range (1024 to 65300)
    while (info.port < 1024 || info.port > 65300) {
        printf("Invalid port. Enter a port between 1024 and 65300");
        return getServer();
    }

    // Return the obtained server information
    return info;
}

#endif // MATH_FUNCTIONS_H

