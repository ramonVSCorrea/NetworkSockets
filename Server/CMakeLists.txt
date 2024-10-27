cmake_minimum_required(VERSION 3.29)
project(Server C)

set(CMAKE_C_STANDARD 11)

add_executable(Server main.c server.c)

# Adiciona a biblioteca ws2_32
target_link_libraries(Server ws2_32)
