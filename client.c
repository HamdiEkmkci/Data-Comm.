#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsaData;
    SOCKET sock;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    char message[BUFFER_SIZE];

    // Winsock baþlatma
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup baþarýsýz oldu.\n");
        return 1;
    }

    // Soket oluþturma
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }

    // Server adresini ayarlama
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Server'a baðlanma
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        return -1;
    }
    
    printf("Yas, kilometre, marka ve fiyati girin (ornek: 5 150000 BMW 120000): ");
    fgets(buffer, sizeof(buffer), stdin);

    // Server'a veri gönderme
    send(sock, buffer, strlen(buffer), 0);

    // Server'dan yanýt alma
    int valread = recv(sock, message, sizeof(message), 0);
    if (valread > 0) {
        message[valread] = '\0';
        printf("Server'dan gelen yanýt: %s\n", message);
    }

    // Soketi kapatma
    closesocket(sock);
    WSACleanup();
    return 0;
}

