#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static client_t clients[MAX_CLIENTS]; // Array para armazenar os clientes conectados
static int client_count = 0; // Contador de clientes conectados
static time_t server_start_time; // Tempo de início do servidor

// Função para enviar uma mensagem de status do servidor
void send_server_status() {
    time_t current_time = time(NULL);
    int elapsed_time = (int)difftime(current_time, server_start_time);
    char status_message[141];
    snprintf(status_message, sizeof(status_message), "Server ID: 0, Clients: %d, Uptime: %d seconds", client_count, elapsed_time);

    msg_t message;
    message.type = htons(MSG_TYPE_MSG);
    message.orig_uid = htons(0); // Identificador do servidor
    message.dest_uid = htons(0); // Broadcast
    message.text_len = htons(strlen(status_message));
    strcpy((char *)message.text, status_message);

    for (int i = 0; i < client_count; i++) {
        send(clients[i].socket, (char *)&message, sizeof(message), 0);
    }
    printf("Status message sent: %s\n", status_message);
}

// Inicia o servidor
void start_server(const char *port) {

    WSADATA wsaData;
    SOCKET listen_socket;
    struct sockaddr_in server_addr;

    // Inicializa o Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup falhou. Error: %d\n", WSAGetLastError());
        return;
    }

    // Cria o socket para escutar conexões
    listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listen_socket == INVALID_SOCKET) {
        printf("Falha ao criar o socket de escuta. Error: %d\n", WSAGetLastError());
        WSACleanup();
        return;
    }

    // Configura o endereço do servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(atoi(port));


    // Associa o socket de escuta ao endereço e porta do servidor
    if (bind(listen_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed. Error: %d\n", WSAGetLastError());
        closesocket(listen_socket);
        WSACleanup();
        return;
    }


    // Coloca o socket de escuta em modo de escuta
    if (listen(listen_socket, SOMAXCONN) == SOCKET_ERROR) {
        printf("Escuta falhou. Error: %d\n", WSAGetLastError());
        closesocket(listen_socket);
        WSACleanup();
        return;
    }

    printf("Servidor iniciado na porta %s\n", port);
    server_start_time = time(NULL);

    fd_set read_fds;
    time_t last_status_time = time(NULL);

    while (1) {
        //printf("Estou aqui\n");
        FD_ZERO(&read_fds);
        FD_SET(listen_socket, &read_fds);
        for (int i = 0; i < client_count; i++) {
            FD_SET(clients[i].socket, &read_fds);
        }

        // Configura o timeout para 1 segundo
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        // Usa o select para monitorar múltiplos sockets com timeout
        int activity = select(0, &read_fds, NULL, NULL, &timeout);
        if( activity == SOCKET_ERROR) {
            printf("Select falhou. Error: %d\n", WSAGetLastError());
            break;
        }
        // if (select(0, &read_fds, NULL, NULL, NULL) == SOCKET_ERROR) {
        //     printf("Select falhou. Error: %d\n", WSAGetLastError());
        //     break;
        // }

        // Aceita novas conexões
        if (FD_ISSET(listen_socket, &read_fds)) {
            SOCKET client_socket = accept(listen_socket, NULL, NULL);
            if (client_socket == INVALID_SOCKET) {
                printf("Aceite falhou. Error: %d\n", WSAGetLastError());
                continue;
            }

            // Adiciona o novo cliente ao array de clientes
            if (client_count < MAX_CLIENTS) {
                clients[client_count].socket = client_socket;
                clients[client_count].uid = 0; // Will be set upon receiving OI message
                client_count++;
                printf("Novo cliente conectado.\n");
            } else {
                printf("Conexão recusada. Limite de clientes atingido!\n");
                closesocket(client_socket);
            }
        }

        // Verifica se há dados para receber de clientes conectados
        for (int i = 0; i < client_count; i++) {
            if (FD_ISSET(clients[i].socket, &read_fds)) {
                msg_t message;
                int bytes_received = recv(clients[i].socket, (char *)&message, sizeof(message), 0);
                if (bytes_received <= 0) {
                    printf("Cliente desconectado.\n");
                    closesocket(clients[i].socket);
                    clients[i] = clients[client_count - 1];
                    client_count--;
                    i--;
                } else {
                    // Processa a mensagem recebida
                    handle_client_message(&clients[i], &message);
                }
            }
        }

        // Envia mensagem de status do servidor a cada minuto
        time_t current_time = time(NULL);
        if(difftime(current_time, last_status_time) >= 60) {
            send_server_status();
            last_status_time = current_time;
        }
    }

    // Fecha o socket de escuta e limpa o Winsock
    closesocket(listen_socket);
    WSACleanup();
}

// Função para processar mensagens recebidas de clientes
void handle_client_message(client_t *client, msg_t *message) {
    switch (ntohs(message->type)) {
        case MSG_TYPE_OI:
            // Define o UID dio cliente e ennvia a mensagem de volta
            client->uid = ntohs(message->orig_uid);
            send(client->socket, (char *)message, sizeof(*message), 0);
            break;
        case MSG_TYPE_TCHAU:
            // Desconecta o cliente
            closesocket(client->socket);
            *client = clients[client_count - 1];
            client_count--;
            break;
        case MSG_TYPE_MSG:
            // Verifica se o UID de origem está correto e retransmite a mensagem
            if (client->uid == ntohs(message->orig_uid)) {
                broadcast_message(message, client);
            }
            break;
        default:
            printf("O tipo de mensagem recebida é desconhecida.\n");
            break;
    }
}

// Função para retransmitir uma mensagem para todos os clientes conectados, exceto o remetente.
void broadcast_message(msg_t *message, client_t *exclude_client) {
    for (int i = 0; i < client_count; i++) {
        if (&clients[i] != exclude_client) {
            send(clients[i].socket, (char *)message, sizeof(*message), 0);
        }
    }
}