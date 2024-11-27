//
// Created by dieeelg on 11/14/24.
//

#ifndef COMANDOSP2_H
#define COMANDOSP2_H

#define TAMANHO 2048
#include "listaMemoriaP2.h"

void Cmd_recurse(char *tr[]);
void Cmd_allocate(char *tr[],tListaMemoria *lista_memoria);
void Cmd_deallocate(char *tr[],tListaMemoria *lista_memoria);
void Cmd_memfill(char *tr[]);
void Cmd_memdump(char *tr[]);
void Cmd_memory(char *tr[],tListaMemoria *lista_memoria);
void Cmd_ReadFile (char *tr[]);
void Cmd_WriteFile (char *tr[]);
void Cmd_Read(char *tr[]);
void Cmd_Write(char *tr[]);


#endif //COMANDOSP2_H
