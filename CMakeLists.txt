cmake_minimum_required(VERSION 3.17)
project(tau_os_ex5 C)

set(CMAKE_C_STANDARD 11)

add_executable(pcc_server pcc_server.c )
add_executable(pcc_client pcc_client.c)