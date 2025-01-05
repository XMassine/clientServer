// Client Code (client.c)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main(int argc, char const *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <server_ip>\n", argv[0]);
        return -1;
    }

    const char *server_ip = argv[1];

    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    // Create socket file descriptor
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        return -1;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        return -1;
    }

    printf("Connected to server at %s\n", server_ip);

    // Receive and display the list of files from the server
    read(sock, buffer, BUFFER_SIZE);
    printf("Available files:\n%s\n", buffer);

    // Prompt user to select a file
    printf("Enter the name of the file to download: ");
    char file_name[BUFFER_SIZE];
    scanf("%s", file_name);

    // Send the file name to the server
    send(sock, file_name, strlen(file_name), 0);

    // Receive and write the file content
    FILE *file = fopen(file_name, "w");
    if (file == NULL) {
        perror("File open failed");
        return -1;
    }

    int bytes_read;
    while ((bytes_read = read(sock, buffer, BUFFER_SIZE)) > 0) {
        fwrite(buffer, sizeof(char), bytes_read, file);
    }

    printf("File '%s' received and saved successfully.\n", file_name);
    fclose(file);
    close(sock);

    return 0;
}
