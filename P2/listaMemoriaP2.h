#ifndef LISTAMEMORIAP2_H
#define LISTAMEMORIAP2_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXDATA 4096

// Estructura para almacenar la información de un archivo
typedef struct archivo {
    int descriptor;                 // Descriptor del archivo
    char nombre[255];               // Nombre del archivo
} tArchivo;

// Enum para diferentes tipos de comandos
typedef enum tipoComando {
    MALLOC, MMAP, SHARED
} tipoComando;

// Estructura para representar un item de memoria (bloque de memoria asignado)
typedef struct tElementoMemoria {
    tipoComando tipo;               // Tipo de comando (malloc, shared, mmap)
    void *direccion;                // Dirección de memoria asignada
    size_t tamaño;                  // Tamaño del bloque de memoria
    char fecha[128];                // Fecha de asignación del bloque
    union {
        int clave;                  // Clave para memoria compartida (si es un bloque compartido)
        tArchivo archivo;           // Información sobre archivo mapeado (si es mmap)
    } informacion;                  // Unión para almacenar diferentes tipos de información
} tElementoMemoria;

// Estructura para representar una lista de bloques de memoria
typedef struct tListaMemoria {
    tElementoMemoria elementos[MAXDATA];  // Array de elementos de memoria
    int posicionUltimo;                    // Índice del último elemento de la lista
} tListaMemoria;

//FUNCIONES DE LA LISTA

void createEmptyListM(tListaMemoria *L);
bool isEmptyListM(tListaMemoria L);
int firstM(tListaMemoria L);
int lastM(tListaMemoria L);
tElementoMemoria getItemM(int pos, tListaMemoria L);
void deleteItemM(int pos, tListaMemoria *L);
void deleteListM(tListaMemoria *L);
bool insertItemM(tElementoMemoria item, tListaMemoria *L);


//FUNCIONES ALLOCATE/DEALLOCATE

void imprimir_malloc(tListaMemoria L);
void imprimir_mmap(tListaMemoria L);
void imprimir_shared(tListaMemoria L);

#endif // LISTAMEMORIAP2_H



