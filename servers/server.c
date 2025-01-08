// Server Code (server.c)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <dirent.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CONNECTIONS 3
#define FILE_LOCATION "."

int main() {
    //init de srv soket clt socket et adress
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    //TCP socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    //ipv4 any network port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    //binding tpc tp adress & port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    //listning for clients
    if (listen(server_fd, MAX_CONNECTIONS) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", PORT);

    //newsocket for mew client connections
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    printf("Connection established with client.\n");

    //ouvrire le repo
    DIR *dir;
    struct dirent *entry;
    if ((dir = opendir(FILE_LOCATION)) == NULL) {
        perror("Could not open directory");
        close(new_socket);
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    
    //iterant les elements de repo
    char file_list[BUFFER_SIZE] = "";
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) { // Only include regular files
            strcat(file_list, entry->d_name);
            strcat(file_list, "\n");
        }
    }
    closedir(dir);

    //envoye des elements
    send(new_socket, file_list, strlen(file_list), 0);

    //lecture de fishier demander attent de client
    memset(buffer, 0, BUFFER_SIZE);
    read(new_socket, buffer, BUFFER_SIZE);
    printf("Requested file: %s\n", buffer);

    //ovrire ce fishier demander
    FILE *file = fopen(buffer, "r");
    if (file == NULL) {
        perror("File open failed");
        close(new_socket);
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    //lecture et envoi de fishier
    int bytes_read;
    while ((bytes_read = fread(buffer, sizeof(char), BUFFER_SIZE, file)) > 0) {
        send(new_socket, buffer, bytes_read, 0);
    }

    printf("File sent successfully.\n");
    fclose(file);
    close(new_socket);
    close(server_fd);

    return 0;
}

