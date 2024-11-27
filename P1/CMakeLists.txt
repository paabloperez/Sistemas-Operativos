cmake_minimum_required(VERSION 3.28.3)
# Nombre del proyecto
project(ShellProject)

# Especificar el estándar de C a usar (opcional)
set(CMAKE_C_STANDARD 11)

# Archivos fuente del proyecto
set(SOURCES
        main.c
        comandosP0.c
        lista.c
        lista.h
        comandosP0.h
        comandosP1.h
        comandosP1.c

)
# Incluir los directorios donde están los archivos de cabecera
include_directories(${CMAKE_SOURCE_DIR})

# Añadir el ejecutable, nombrándolo por ejemplo "shell"
add_executable(P1 ${SOURCES})



