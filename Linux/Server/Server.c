#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static client_t
    clients[MAX_CLIENTS];         // Array para armazenar os clientes conectados
static int client_count = 0;      // Contador de clientes conectados
static time_t server_start_time;  // Tempo de início do servidor

void send_server_status() {
  time_t current_time = time(NULL);
  int elapsed_time = (int)difftime(current_time, server_start_time);
  char status_message[141];
  snprintf(status_message, sizeof(status_message),
           "Server ID: 0, Clients: %d, Uptime: %d seconds", client_count,
           elapsed_time);

  msg_t message;
  message.type = htons(MSG_TYPE_MSG);
  message.orig_uid = htons(0);
  message.dest_uid = htons(0);
  message.text_len = htons(strlen(status_message));
  strcpy((char*)message.text, status_message);

  for (int i = 0; i < client_count; i++) {
    send(clients[i].socket, &message, sizeof(message), 0);
  }
  printf("Status message sent: %s\n", status_message);
}

void start_server(const char* port) {
  int listen_socket;
  struct sockaddr_in server_addr;

  listen_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_socket < 0) {
    perror("Socket creation failed");
    return;
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(atoi(port));

  if (bind(listen_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) <
      0) {
    perror("Bind failed");
    close(listen_socket);
    return;
  }

  if (listen(listen_socket, SOMAXCONN) < 0) {
    perror("Listen failed");
    close(listen_socket);
    return;
  }

  printf("Server started on port %s\n", port);
  server_start_time = time(NULL);

  fd_set read_fds;
  time_t last_status_time = time(NULL);

  while (1) {
    FD_ZERO(&read_fds);
    FD_SET(listen_socket, &read_fds);
    for (int i = 0; i < client_count; i++) {
      FD_SET(clients[i].socket, &read_fds);
    }

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    int activity = select(FD_SETSIZE, &read_fds, NULL, NULL, &timeout);
    if (activity < 0) {
      perror("Select failed");
      break;
    }

    if (FD_ISSET(listen_socket, &read_fds)) {
      int client_socket = accept(listen_socket, NULL, NULL);
      if (client_socket < 0) {
        perror("Accept failed");
        continue;
      }

      if (client_count < MAX_CLIENTS) {
        clients[client_count].socket = client_socket;
        clients[client_count].uid = 0;
        client_count++;
        printf("New client connected.\n");
      } else {
        printf("Connection refused. Client limit reached!\n");
        close(client_socket);
      }
    }

    for (int i = 0; i < client_count; i++) {
      if (FD_ISSET(clients[i].socket, &read_fds)) {
        msg_t message;
        int bytes_received =
            recv(clients[i].socket, &message, sizeof(message), 0);
        if (bytes_received <= 0) {
          printf("Client disconnected.\n");
          close(clients[i].socket);
          clients[i] = clients[client_count - 1];
          client_count--;
          i--;
        } else {
          handle_client_message(&clients[i], &message);
        }
      }
    }

    time_t current_time = time(NULL);
    if (difftime(current_time, last_status_time) >= 60) {
      send_server_status();
      last_status_time = current_time;
    }
  }

  close(listen_socket);
}

void handle_client_message(client_t* client, msg_t* message) {
  switch (ntohs(message->type)) {
    case MSG_TYPE_OI:
      client->uid = ntohs(message->orig_uid);
      send(client->socket, message, sizeof(*message), 0);
      break;
    case MSG_TYPE_TCHAU:
      close(client->socket);
      *client = clients[client_count - 1];
      client_count--;
      break;
    case MSG_TYPE_MSG:
      if (client->uid == ntohs(message->orig_uid)) {
        broadcast_message(message, client);
      }
      break;
    default:
      printf("Unknown message type received.\n");
      break;
  }
}

void broadcast_message(msg_t* message, client_t* exclude_client) {
  for (int i = 0; i < client_count; i++) {
    if (&clients[i] != exclude_client) {
      send(clients[i].socket, message, sizeof(*message), 0);
    }
  }
}