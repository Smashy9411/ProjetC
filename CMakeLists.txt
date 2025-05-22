cmake_minimum_required(VERSION 3.25)
project(ProjetC)

set(CMAKE_C_STANDARD 11)

# Liste EXPLICITE de tous les fichiers sources
add_executable(ProjetC
        main.c
        bmp8.c
        bmp24.c
)

# Option pour forcer la détection des fichiers
set(CMAKE_INCLUDE_CURRENT_DIR ON)