cmake_minimum_required(VERSION 3.29)
project(Server C)

set(CMAKE_C_STANDARD 11)


add_executable(Server main.c Server.c)

# Adiciona a biblioteca pthread
target_link_libraries(Server pthread)