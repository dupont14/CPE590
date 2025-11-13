// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 9000
#define BUFFER_SIZE 4096

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    socklen_t addr_len = sizeof(address);
    char buffer[BUFFER_SIZE];
    FILE *fp;

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Bind socket to port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // listen on all interfaces
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // Listen for connection
    if (listen(server_fd, 1) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept a connection
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, &addr_len)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    printf("Connection established from %s\n", inet_ntoa(address.sin_addr));

    // Receive filename
    char filename[256];
    int bytes_read = recv(new_socket, filename, sizeof(filename), 0);
    if (bytes_read <= 0) {
        perror("recv filename");
        close(new_socket);
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    filename[bytes_read] = '\0';
    send(new_socket, "OK", 2, 0); // acknowledge

    // Open file for writing
    fp = fopen(filename, "wb");
    if (!fp) {
        perror("fopen");
        close(new_socket);
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Receiving file: %s\n", filename);

    // Receive file data
    ssize_t n;
    while ((n = recv(new_socket, buffer, BUFFER_SIZE, 0)) > 0) {
        fwrite(buffer, 1, n, fp);
    }

    printf("File received successfully.\n");

    fclose(fp);
    close(new_socket);
    close(server_fd);
    return 0;
}
