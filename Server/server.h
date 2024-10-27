#ifndef SERVER_H
#define SERVER_H

#include <winsock2.h>
#include "msg_protocol.h"

#define MAX_CLIENTS 100

/**
 * @brief Estrutura que representa um cliente conectado.
 */
typedef struct {
    SOCKET socket;
    uint16_t uid;
} client_t;

void send_server_status();

/**
 * @brief Inicia o servidor na porta especificada.
 *
 * @param port A porta na qual o servidor deve escutar.
 */
void start_server(const char *port);

/**
 * @brief Processa uma mensagem recebida de um cliente.
 *
 * @param client O cliente que enviou a mensagem.
 * @param message A mensagem recebida.
 */
void handle_client_message(client_t *client, msg_t *message);

/**
 * @brief Retransmite uma mensagem para todos os clientes conectados
 *
 * @param message A mensagem a ser retransmitida.
 * @param exclude_client O cliente que não deve receber a mensagem.
 */
void broadcast_message(msg_t *message, client_t *exclude_client);

#endif //SERVER_H
