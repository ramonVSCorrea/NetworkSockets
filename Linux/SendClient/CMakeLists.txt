cmake_minimum_required(VERSION 3.29)
project(SendClient C)

set(CMAKE_C_STANDARD 11)

add_executable(SendClient main.c
        SendClient.c
        SendClient.h)