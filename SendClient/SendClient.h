#ifndef SENDCLIENT_H
#define SENDCLIENT_H

#include <winsock2.h>
#include "msg_protocol.h"
#include <stdint.h>

/**
 * @brief Inicia o cliente de envio de mensagens.
 *
 * @param server_addr O endereço do servidor.
 * @param port A porta do servidor.
 * @param uid O identificador do cliente.
 */
void start_send_client(const char *server_addr, const char *port, uint16_t uid);


#endif //SENDCLIENT_H
