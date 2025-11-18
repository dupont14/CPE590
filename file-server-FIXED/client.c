// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 9000
#define BUFFER_SIZE 4096

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <server_ip> <file_path>\n", argv[0]);
        return 1;
    }

    char *server_ip = argv[1];
    char *file_path = argv[2];
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    FILE *fp;

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    // Extract filename from path
    char *filename = strrchr(file_path, '/');
    filename = filename ? filename + 1 : file_path;

    // Send filename
    send(sock, filename, strlen(filename), 0);

    // Wait for "OK"
    recv(sock, buffer, sizeof(buffer), 0);

    // Open file for writing
    fp = fopen(filename, "wb");
    if (!fp) {
        perror("fopen");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("Receiving file: %s\n", filename);

    // Receive file data
    ssize_t n;
    while ((n = recv(sock, buffer, BUFFER_SIZE, 0)) > 0) {
        fwrite(buffer, 1, n, fp);
    }

    printf("File sent successfully.\n");

    fclose(fp);
    close(sock);
    return 0;
}
