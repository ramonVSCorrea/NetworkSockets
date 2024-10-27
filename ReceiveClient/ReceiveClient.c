#include "ReceiveClient.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void start_receive_client(const char *server_addr, const char *port, uint16_t uid) {
    WSADATA wsaData;
    SOCKET sock;
    struct sockaddr_in server;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed. Error: %d\n", WSAGetLastError());
        return;
    }

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        printf("Socket creation failed. Error: %d\n", WSAGetLastError());
        WSACleanup();
        return;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(server_addr);
    server.sin_port = htons(atoi(port));

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Connection failed. Error: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return;
    }

    msg_t message;
    message.type = htons(MSG_TYPE_OI);
    message.orig_uid = htons(uid);
    message.dest_uid = 0;
    message.text_len = 0;

    send(sock, (char *)&message, sizeof(message), 0);

    int bytes_received = recv(sock, (char *)&message, sizeof(message), 0);
    if (bytes_received <= 0 || ntohs(message.type) != MSG_TYPE_OI) {
        printf("Failed to receive OI response.\n");
        closesocket(sock);
        WSACleanup();
        return;
    }

    printf("Connected to server. Waiting for messages...\n");

    while (1) {
        bytes_received = recv(sock, (char *)&message, sizeof(message), 0);
        if (bytes_received <= 0) {
            printf("Connection lost.\n");
            break;
        }

        if (ntohs(message.type) == MSG_TYPE_MSG) {
            printf("Message from %hu: %s\n", ntohs(message.orig_uid), message.text);
        }
    }

    closesocket(sock);
    WSACleanup();
}