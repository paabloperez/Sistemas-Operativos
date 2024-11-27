#include "lista.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Inicializar la lista de historial
void CreateHistList(HistList *list) {
    list->head = NULL;  // Inicializamos la cabeza de la lista como NULL
    list->size = 0;     // El tamaño de la lista comienza en 0
}

// Insertar un nuevo comando en la lista de historial
/*void InsertHistElement(HistList *list, char *tokens[]) {
    HistNode *newNode = malloc(sizeof(HistNode));

    // Copiar los tokens en el nuevo nodo
    for (int i = 0; i < MAX && tokens[i] != NULL; i++) {
        newNode->trozos[i] = strdup(tokens[i]);  // Usar strdup para copiar el token
    }

    // Inicializar el resto del array de tokens a NULL
    for (int i = 0; i < MAX && tokens[i] == NULL; i++) {
        newNode->trozos[i] = NULL;
    }

    // Insertar el nuevo nodo al principio de la lista
    newNode->next = list->head;
    list->head = newNode;
    list->size++;
}*/

void InsertHistElement(HistList *list, char *tokens[]) {
    // Crear un nuevo nodo
    HistNode *newNode = malloc(sizeof(HistNode));

    // Verificar si la asignación de memoria fue exitosa
    if (newNode == NULL) {
        fprintf(stderr, "Error: No se pudo asignar memoria.\n");
        return;
    }

    int i;
    // Copiar los tokens en el nuevo nodo
    for (i = 0; i < MAX && tokens[i] != NULL; i++) {
        newNode->trozos[i] = strdup(tokens[i]);  // Usar strdup para copiar el token
    }

    // Inicializar el resto del array de tokens a NULL
    for (; i < MAX; i++) {
        newNode->trozos[i] = NULL;
    }

    // Insertar el nuevo nodo al principio de la lista
    newNode->next = list->head;
    list->head = newNode;
    list->size++;
}



// Imprimir todos los comandos en el historial
void PrintHistList(HistList *list) {
    HistNode *current = list->head;
    int count = list->size;

    while (current != NULL) {
        printf("(%d) ", count);  // Imprimir el número de comando
        for (int i = 0; i < count && current->trozos[i] != NULL; i++) {
            printf("%s ", current->trozos[i]);  // Imprimir cada token del comando
        }
        printf("\n");
        current = current->next;
        count--;
    }
}

// Imprimir los últimos 'n' comandos del historial
void PrintLastNCommands(HistList *list, int n) {
    if (list->size == 0 || n <= 0) {
        printf("No hay comandos para mostrar.\n");
        return;
    }

    // Ajustar n si es mayor que el tamaño de la lista
    if (n > list->size) {
        n = list->size;
    }

    HistNode *current = list->head;
    int count = 0;

    while (current != NULL && count < n) {
        for (int i = 0; i < MAX && current->trozos[i] != NULL; i++) {
            printf("%s ", current->trozos[i]);  // Imprimir cada token del comando
        }
        printf("\n");
        current = current->next;
        count++;
    }
}

// Obtener un comando por su número en el historial
HistNode* GetCommandByNumber(HistList *list, int n) {
    if (n > list->size || n <= 0) {
        printf("Número de comando inválido.\n");
        return NULL;
    }

    HistNode *current = list->head;
    int count = list->size;

    // Buscar el comando correspondiente
    while (current != NULL && count > n) {
        current = current->next;
        count--;
    }

    return current;  // Devolver el nodo con el comando correspondiente
}

// Liberar toda la memoria usada por la lista de historial
void FreeHistList(HistList *list) {
    HistNode *current = list->head;

    while (current != NULL) {
        HistNode *next = current->next;

        // Liberar cada token en el nodo
        for (int i = 0; i < MAX && current->trozos[i] != NULL; i++) {
            free(current->trozos[i]);  // Liberar cada cadena de comandos
        }

        // Liberar el nodo en sí
        free(current);

        current = next;
    }

    // Finalmente, marcar la lista como vacía
    list->head = NULL;
    list->size = 0;
}
