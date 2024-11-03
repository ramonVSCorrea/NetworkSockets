#ifndef SERVER_H
#define SERVER_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include "msg_protocol.h"

#define MAX_CLIENTS 100

typedef struct {
  int socket;
  uint16_t uid;
} client_t;

void send_server_status();
void start_server(const char* port);
void handle_client_message(client_t* client, msg_t* message);
void broadcast_message(msg_t* message, client_t* exclude_client);

#endif  // SERVER_H