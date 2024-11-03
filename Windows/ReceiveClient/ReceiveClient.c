#include "ReceiveClient.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Função que inicia o cliente de recebimento de mensagens
void start_receive_client(const char *server_addr, const char *port, uint16_t uid) {
    WSADATA wsaData; // Estrutura para armazenar informações sobre a implementação do Winsock
    SOCKET sock; // Descritor de socket
    struct sockaddr_in server; // Estrutura para armazenar informações sobre o servidor

    // Inicializa o Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Falha no WSAStartup. Erro: %d\n", WSAGetLastError());
        return;
    }

    // Cria um socket
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        printf("Falha na criação do socket. Erro: %d\n", WSAGetLastError());
        WSACleanup();
        return;
    }

    // Configura a estrutura de endereço do servidor
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(server_addr);
    server.sin_port = htons(atoi(port));

    // Conecta ao servidor
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Falha na conexão. Erro: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return;
    }

    // Prepara a mensagem de identificação inicial
    msg_t message;
    message.type = htons(MSG_TYPE_OI);
    message.orig_uid = htons(uid);
    message.dest_uid = 0;
    message.text_len = 0;

    // Envia a mensagem de identificação inicial
    send(sock, (char *)&message, sizeof(message), 0);

    // Envia a mensagem de identificação inicial
    int bytes_received = recv(sock, (char *)&message, sizeof(message), 0);
    if (bytes_received <= 0 || ntohs(message.type) != MSG_TYPE_OI) {
        printf("Falha ao receber resposta OI.\n");
        closesocket(sock);
        WSACleanup();
        return;
    }

    printf("Conectado ao servidor. Aguardando mensagens...\n");

    // Loop principal para receber mensagens do servidor
    while (1) {
        bytes_received = recv(sock, (char *)&message, sizeof(message), 0);
        if (bytes_received <= 0) {
            printf("Conexão perdida.\n");
            break;
        }

        // Verifica se a mensagem recebida é uma mensagem de texto
        if (ntohs(message.type) == MSG_TYPE_MSG) {
            printf("Mensagem de %hu: %s\n", ntohs(message.orig_uid), message.text);
        }
    }

    // Limpa e fecha o socket
    closesocket(sock);
    WSACleanup();
}