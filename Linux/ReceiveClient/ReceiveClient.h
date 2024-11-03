#ifndef RECEIVECLIENT_H
#define RECEIVECLIENT_H

#include <stdint.h>
#include "msg_protocol.h"

/**
 * @brief Inicia o cliente de recepção de mensagens.
 *
 * @param server_addr O endereço do servidor.
 * @param port A porta do servidor.
 * @param uid O identificador do cliente.
 */
void start_receive_client(const char* server_addr,
                          const char* port,
                          uint16_t uid);

#endif  // RECEIVECLIENT_H