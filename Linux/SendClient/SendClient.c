#include "SendClient.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Função que inicia o cliente de envio de mensagens
void start_send_client(const char* server_addr,
                       const char* port,
                       uint16_t uid) {
  int sock;  // Descritor de socket
  struct sockaddr_in
      server;  // Estrutura para armazenar informações sobre o servidor

  // Cria um socket
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    perror("Criação do socket falhou");
    return;
  }

  // Configura a estrutura sockaddr_in com informações sobre o servidor
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr(server_addr);
  server.sin_port = htons(atoi(port));

  // Conecta ao servidor
  if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
    perror("Conexão falhou");
    close(sock);
    return;
  }

  // Prepara a mensagem de saudação (OI)
  msg_t message;
  message.type = htons(MSG_TYPE_OI);
  message.orig_uid = htons(uid);
  message.dest_uid = 0;
  message.text_len = 0;

  // Envia a mensagem de saudação (OI) ao servidor
  send(sock, &message, sizeof(message), 0);

  // Recebe a resposta do servidor
  int bytes_received = recv(sock, &message, sizeof(message), 0);
  if (bytes_received <= 0 || ntohs(message.type) != MSG_TYPE_OI) {
    printf("Falha ao receber resposta OI.\n");
    close(sock);  // Fecha o socket
    return;
  }

  printf("Conectado ao servidor. Você pode começar a enviar mensagens.\n");

  // Loop para enviar mensagens
  while (1) {
    char text[141];     // Buffer para armazenar a mensagem
    uint16_t dest_uid;  // UID do destinatário

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
    strcpy((char*)message.text, text);

    // Envia a mensagem ao servidor
    send(sock, &message, sizeof(message), 0);
  }

  // Fecha o socket
  close(sock);
}