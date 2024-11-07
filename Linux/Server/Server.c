#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static client_t
    clients[MAX_CLIENTS];         // Array para armazenar os clientes conectados
static int client_count = 0;      // Contador de clientes conectados
static time_t server_start_time;  // Tempo de início do servidor
volatile sig_atomic_t timer_expired =
    0;  // Flag para indicar que o timer expirou

// Manipulador de sinal para SIGALRM
void handle_alarm(int sig) {
  (void)sig;  // Suprime o aviso de parâmetro não utilizado
  timer_expired = 1;
}

// Configura o timer para expirar a cada 60 segundos
void setup_timer() {
  struct sigaction sa;
  struct itimerval timer;

  // Configura o handler para o sinal SIGALRM
  sa.sa_handler = &handle_alarm;
  sa.sa_flags = SA_RESTART;
  sigaction(SIGALRM, &sa, NULL);

  // Configura o timer para expirar a cada 60 segundos
  timer.it_value.tv_sec = 60;
  timer.it_value.tv_usec = 0;
  timer.it_interval.tv_sec = 60;
  timer.it_interval.tv_usec = 0;
  setitimer(ITIMER_REAL, &timer, NULL);
}

// Função para enviar uma mensagem de status do servidor
void send_server_status() {
  char status_message[141];
  snprintf(status_message, sizeof(status_message),
           "Servidor ativo há %ld segundos com %d clientes conectados.",
           time(NULL) - server_start_time, client_count);

  msg_t message;
  message.type = htons(MSG_TYPE_MSG);
  message.orig_uid = htons(0);  // UID do servidor
  message.dest_uid = htons(0);  // Broadcast
  message.text_len = htons(strlen(status_message));
  strcpy((char*)message.text, status_message);

  // Envia a mensagem de status para todos os clientes conectados
  for (int i = 0; i < client_count; i++) {
    send(clients[i].socket, &message, sizeof(message), 0);
  }
  printf("Mensagem de status enviada: %s\n", status_message);
}

// Função para processar mensagens dos clientes
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
      printf("Tipo de mensagem desconhecido.\n");
      break;
  }
}

// Função para enviar uma mensagem para todos os clientes conectados
void broadcast_message(msg_t* message, client_t* sender) {
  for (int i = 0; i < client_count; i++) {
    if (clients[i].uid != sender->uid) {
      send(clients[i].socket, message, sizeof(*message), 0);
    }
  }
}

// Função para verificar se um UID já está em uso
int is_uid_in_use(uint16_t uid) {
  for (int i = 0; i < client_count; i++) {
    if (clients[i].uid == uid) {
      return 1;
    }
  }
  return 0;
}

// Função para iniciar o servidor
void start_server(const char* port) {
  int listen_socket;
  struct sockaddr_in server_addr;

  // Cria um socket para escutar conexões
  listen_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_socket < 0) {
    perror("Falha ao criar o socket");
    return;
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(atoi(port));

  // Associa o socket ao endereço e porta especificados
  if (bind(listen_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) <
      0) {
    perror("Falha ao associar (bind)");
    close(listen_socket);
    return;
  }

  // Coloca o socket em modo de escuta para aceitar conexões
  if (listen(listen_socket, SOMAXCONN) < 0) {
    perror("Falha ao escutar (listen)");
    close(listen_socket);
    return;
  }

  printf("Servidor iniciado na porta %s\n", port);
  server_start_time = time(NULL);

  fd_set read_fds;

  // Configura o timer para enviar mensagens de status periodicamente
  setup_timer();

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
      if (errno == EINTR) {
        // Se o select foi interrompido por um sinal, continue
        continue;
      } else {
        perror("Erro no select");
        break;
      }
    }

    if (timer_expired) {
      send_server_status();
      timer_expired = 0;
    }

    if (FD_ISSET(listen_socket, &read_fds)) {
      // Aceita novas conexões
      int new_socket = accept(listen_socket, NULL, NULL);
      if (new_socket < 0) {
        perror("Falha ao aceitar conexão");
        continue;
      }

      // Recebe a mensagem de saudação (OI) do novo cliente
      msg_t message;
      int bytes_received = recv(new_socket, &message, sizeof(message), 0);
      if (bytes_received <= 0 || ntohs(message.type) != MSG_TYPE_OI) {
        printf("Falha ao receber mensagem OI.\n");
        close(new_socket);
        continue;
      }

      uint16_t new_uid = ntohs(message.orig_uid);
      if (is_uid_in_use(new_uid) ||
          (new_uid <= 999 && is_uid_in_use(new_uid + 1000))) {
        printf("UID %hu já está em uso. Conexão recusada.\n", new_uid);
        message.type = htons(MSG_TYPE_ERRO);
        send(new_socket, &message, sizeof(message), 0);
        close(new_socket);
        continue;
      }

      if (client_count < MAX_CLIENTS) {
        clients[client_count].socket = new_socket;
        clients[client_count].uid = new_uid;
        client_count++;
        printf("Novo cliente conectado com UID %hu.\n", new_uid);

        // Envia a resposta "OI" de volta ao cliente
        msg_t oi_response;
        oi_response.type = htons(MSG_TYPE_OI);
        oi_response.orig_uid = htons(0);  // UID do servidor
        oi_response.dest_uid = htons(new_uid);
        oi_response.text_len = htons(0);
        send(new_socket, &oi_response, sizeof(oi_response), 0);
      } else {
        printf("Número máximo de clientes atingido. Conexão recusada.\n");
        close(new_socket);
      }
    }

    // Lida com clientes existentes
    for (int i = 0; i < client_count; i++) {
      if (FD_ISSET(clients[i].socket, &read_fds)) {
        msg_t message;
        int bytes_received =
            recv(clients[i].socket, &message, sizeof(message), 0);
        if (bytes_received <= 0) {
          printf("Cliente desconectado.\n");
          close(clients[i].socket);
          clients[i] = clients[client_count - 1];
          client_count--;
          i--;
        } else {
          handle_client_message(&clients[i], &message);
        }
      }
    }
  }

  close(listen_socket);
}