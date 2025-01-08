// Client Code (client.c)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024


int main(int argc, char const *argv[]) {
    //assure quon a le ip server
    if (argc != 2) {
        printf("Usage: %s <server_ip>\n", argv[0]);
        return -1;
    }

    const char *server_ip = argv[1];

    //init socket & badress de server
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    //create TCP spcket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }

    //config de server
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    //cenvert ip (v4 or v6) to binary
    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        return -1;
    }

    //connection au server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        return -1;
    }

    printf("Connected to server at %s\n", server_ip);

    //listes des element de repo server
    read(sock, buffer, BUFFER_SIZE);
    printf("Available files:\n%s\n", buffer);

    //lecture de nom de fishier demander par lutilisateur
    printf("Enter the name of the file to download: ");
    char file_name[BUFFER_SIZE];
    scanf("%s", file_name);

    //envoi de nom de fishier
    send(sock, file_name, strlen(file_name), 0);

    //reception de fishier
    FILE *file = fopen(file_name, "w");
    if (file == NULL) {
        perror("File open failed");
        return -1;
    }

    //lecture en chunks de fishir
    int bytes_read;
    while ((bytes_read = read(sock, buffer, BUFFER_SIZE)) > 0) {
        fwrite(buffer, sizeof(char), bytes_read, file);
    }

    printf("File '%s' received and saved successfully.\n", file_name);
    fclose(file);
    close(sock);

    return 0;
}
