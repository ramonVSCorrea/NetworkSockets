#include "ReceiveClient.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Função que inicia o cliente de recebimento de mensagens
void start_receive_client(const char* server_addr,
                          const char* port,
                          uint16_t uid) {
  int sock;  // Descritor de socket
  struct sockaddr_in
      server;  // Estrutura para armazenar informações sobre o servidor

  // Cria um socket
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    perror("Falha na criação do socket");
    return;
  }

  // Configura a estrutura de endereço do servidor
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr(server_addr);
  server.sin_port = htons(atoi(port));

  // Conecta ao servidor
  if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
    perror("Falha na conexão");
    close(sock);
    return;
  }

  // Prepara a mensagem de identificação inicial
  msg_t message;
  message.type = htons(MSG_TYPE_OI);
  message.orig_uid = htons(uid);
  message.dest_uid = 0;
  message.text_len = 0;

  // Envia a mensagem de identificação inicial
  send(sock, &message, sizeof(message), 0);

  // Recebe a resposta do servidor
  int bytes_received = recv(sock, &message, sizeof(message), 0);
  if (bytes_received <= 0 || ntohs(message.type) != MSG_TYPE_OI) {
    printf("Falha ao receber resposta OI.\n");
    close(sock);
    return;
  }

  printf("Conectado ao servidor. Aguardando mensagens...\n");

  // Loop principal para receber mensagens do servidor
  while (1) {
    bytes_received = recv(sock, &message, sizeof(message), 0);
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
  close(sock);
}