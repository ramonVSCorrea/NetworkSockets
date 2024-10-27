cmake_minimum_required(VERSION 3.29)
project(ReceiveClient C)

set(CMAKE_C_STANDARD 11)

add_executable(ReceiveClient main.c
        ReceiveClient.c
        ReceiveClient.h)

# Adiciona a biblioteca ws2_32
target_link_libraries(ReceiveClient ws2_32)