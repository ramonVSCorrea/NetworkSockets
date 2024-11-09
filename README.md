# Projeto de Sistema de Mensagens Multi-Servidor

## Descrição

 Este projeto implementa um sistema de encio de mensagens curtas (um mini-X) que funciona em um modelo multi-servidor. O sistema é composto por três programas principais:

1. **Servidor**: Responsável pelo controle da troca de mensagens entre os clientes.
2. **Cliente de Envio**: Programa cliente que envia mensagens para o servidor.
3. **Cliente de Recepção**: Programa cliente que recebe mensagens do servidor.

O servidor aceita conexões de múltiplos clientes, gerencia essas conexões usando a primitiva `select`, processa mensagens dos clientes e envia periodicamente mensagens de status para todos os clientes conectados.

## Estrutura do Projeto

O projeto está organizado da seguinte forma:

```.
├── Server
│   ├── CMakeLists.txt
│   ├── Makefile
│   ├── main.c
│   ├── server.c
│   ├── server.h
│   └── msg_protocol.h
├── SendClient
│   ├── CMakeLists.txt
│   ├── Makefile
│   ├── main.c
│   ├── SendClient.c
│   ├── SendClient.h
│   └── msg_protocol.h
└── ReceiveClient
    ├── CMakeLists.txt
    ├── Makefile
    ├── main.c
    ├── ReceiveClient.c
    ├── ReceiveClient.h
    └── msg_protocol.h
```

## Compilação e execução do projeto

Para compilar o projeto, temos um `Makefile` em cada diretório que facilita a compilação. Para compilar o projeto, basta executar o comando `make` em cada diretório. Para executar o projeto basta executar o comando `./<Nome_Do_Projeto>`. A seguir temos os comandos para cada um dos projetos:

 ### Servidor

- Navegue até o diretório `Server`:

```bash
cd Server
```

- Compile o projeto:

```bash
make
```

- Execute o servidor:

```bash
./Server
```

### Cliente de Envio

- Navegue até o diretório `SendClient`:

```bash
cd SendClient
```

- Compile o projeto:

```bash
make
```

- Execute o cliente de envio:

```bash
./SendClient
```

### Cliente de Exibição

- Navegue até o diretório `ReceiveClient`:

```bash
cd ReceiveClient
```

- Compile o projeto:

```bash
make
```

- Execute o cliente de exibição:

```bash
./ReceiveClient
```

