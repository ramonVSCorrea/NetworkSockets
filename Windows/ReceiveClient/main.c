#include <stdio.h>
#include "ReceiveClient.h"

int main(int argc, char *argv[]) {
    // Valores padrão para o endereço do servidor, porta e UID
    const char *default_server_addr = "127.0.0.1"; // Endereço do servidor
    const char *default_port = "8080"; // Porta
    const char *default_uid = "1001"; // UID

    // Verifica se o número de argumentos é diferente de 4
    if (argc != 4) {
        // Exibe a mensagem de uso e os valores padrão
        printf("Uso: %s <endereco_servidor> <porta> <uid>\n", argv[0]);
        printf("Usando valores padrão: %s %s %s\n", default_server_addr, default_port, default_uid);

        // Define os valores padrão nos argumentos
        argv[1] = (char *) default_server_addr;
        argv[2] = (char *) default_port;
        argv[3] = (char *) default_uid;

        argc = 4; // Ajusta o número de argumentos para 4
    }

    // Obtém os valores dos argumentos
    const char *server_addr = argv[1];
    const char *port = argv[2];
    uint16_t uid = atoi(argv[3]);

    // Verifica se o UID está no intervalo correto
    if (uid < 1001 || uid > 1999) {
        printf("UID deve estar entre 1001 e 1999.\n");
        return 1; // Retorna um código de erro
    }

    // Inicia o cliente de recepção de mensagens
    start_receive_client(server_addr, port, uid);
    return 0;
}
