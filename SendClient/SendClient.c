#include "SendClient.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Função que inicia o cliente de envio de mensagens
void start_send_client(const char *server_addr, const char *port, uint16_t uid) {
    WSADATA wsaData; // Estrutura para armazenar informações sobre a implementação do Winsock
    SOCKET sock; // Descritor de socket
    struct sockaddr_in server; // Estrutura para armazenar informações sobre o servidor

    // Inicializa o uso do Winsock DLL
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup falhou. Error: %d\n", WSAGetLastError());
        return;
    }

    // Cria um socket
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        printf("Criação do socket falhou. Error: %d\n", WSAGetLastError());
        WSACleanup();
        return;
    }

    // Configura a estrutura sockaddr_in com informações sobre o servidor
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(server_addr);
    server.sin_port = htons(atoi(port));

    // Conecta ao servidor
    if (connect(sock, (struct sockaddr *) &server, sizeof(server)) == SOCKET_ERROR) {
        printf("Conexão falhou. Error: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return;
    }

    // Prepara a mensagem de saudação (OI)
    msg_t message;
    message.type = htons(MSG_TYPE_OI);
    message.orig_uid = htons(uid);
    message.dest_uid = 0;
    message.text_len = 0;

    // Envia a mensagem de saudação (OI) ao servidor
    send(sock, (char *) &message, sizeof(message), 0);

    // Recebe a resposta do servidor
    int bytes_received = recv(sock, (char *) &message, sizeof(message), 0);
    if (bytes_received <= 0 || ntohs(message.type) != MSG_TYPE_OI) {
        printf("Falha ao receber resposta OI.\n");
        closesocket(sock); // Fecha o socket
        WSACleanup(); // Libera os recursos alocados pelo WSAStartup
        return;
    }

    printf("Conectado ao servidor. Você pode começar a enviar mensagens.\n");

    // Loop para enviar mensagens
    while (1) {
        char text[141]; // Buffer para armazenar a mensagem
        uint16_t dest_uid; // UID do destinatário

        // Solicita o UID de destino do usuário
        printf("Digite o UID de destino (0 para broadcast): ");
        scanf("%hu", &dest_uid);

        // Solicita a mensagem ao usuário
        printf("Digite sua mensagem: ");
        scanf(" %[^\n]", text);

        // Prepara a mensagem a ser enviada
        message.type = htons(MSG_TYPE_MSG);
        message.orig_uid = htons(uid);
        message.dest_uid = htons(dest_uid);
        message.text_len = htons(strlen(text));
        strcpy((char *) message.text, text);

        //Envia a mensagem ao servidor
        send(sock, (char *) &message, sizeof(message), 0);
    }

    // Fecha o socket e libera os recursos alocados pelo WSAStartup
    closesocket(sock);
    WSACleanup();
}
