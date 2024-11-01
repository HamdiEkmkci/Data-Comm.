#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void process_request(char *request, char *response) {
    int age, km, price;
    char brand[50];
    int should_exit = 0;
    
    // Gelen bilgiyi ayristiriyoruz (age, km, brand, price)
    sscanf(request, "%d %d %s %d", &age, &km, brand, &price);

    // Kurallar
    if (age > 10) {
        strcpy(response, "Satis reddedildi: Arabanin yasi 10'dan buyuk.");
    } else if (km > 200000) {
        strcpy(response, "Satis reddedildi: Arabanin kilometresi 200.000'den fazla.");
    } else if (strcmp(brand, "BMW") == 0) {
        strcpy(response, "Satis kabul edildi: BMW satisa uygun.");
    } else if (price < 100000) {
        strcpy(response, "Satis reddedildi: Fiyat 100.000 TL'den dusuk.");
    } else {
        strcpy(response, "Satis kabul edildi.");
    }
}

int main() {
    WSADATA wsaData;
    SOCKET server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    char response[BUFFER_SIZE] = {0};

    // Winsock baslatma
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup basarisiz oldu.\n");
        return 1;
    }

    // Soket olusturma
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket olusturulamadi: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Baðlama
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
        printf("Bind hatasi: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    // Dinleme
    if (listen(server_fd, 3) == SOCKET_ERROR) {
        printf("Dinleme hatasi: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    printf("Server baslatildi ve port %d uzerinde dinleniyor...\n", PORT);

    while (!should_exit) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) == INVALID_SOCKET) {
            printf("Kabul hatasi: %d\n", WSAGetLastError());
            closesocket(server_fd);
            WSACleanup();
            return 1;
        }

        // Client'tan veri alma
        int valread = recv(new_socket, buffer, BUFFER_SIZE, 0);
        if (valread > 0) {
            buffer[valread] = '\0'; // String sonlandirma
            printf("Ýstek alindi: %s\n", buffer);
            
            if (strcmp(buffer, "exit") == 0) {
                printf("Sunucu kapatiliyor...\n");
                should_exit = 1;
                strcpy(response, "Sunucu kapatiliyor...");
                send(new_socket, response, strlen(response), 0);
                closesocket(new_socket);
                break;
            }

            process_request(buffer, response);
            send(new_socket, response, strlen(response), 0);
            printf("Yanit gonderildi: %s\n", response);
        }

        closesocket(new_socket);
    }

    closesocket(server_fd);
    WSACleanup();
    return 0;
}

