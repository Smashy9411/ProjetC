cmake_minimum_required(VERSION 3.10)
project(ProjetC C)

set(CMAKE_C_STANDARD 99)

add_executable(main

        main.c
        bmp8.c
        bmp24.c
)
