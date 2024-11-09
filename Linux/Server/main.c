#include "server.h"

int main() {
  char port[6] = "8080";  // Número da porta que deseja utilizar para o servidor

  // Inicia o servidor
  start_server(port);

  return 0;
}