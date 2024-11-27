#ifndef COMANDOS_H
#define COMANDOS_H

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include "lista.h"

#define MAX 1024
#define MAX_CMD 4096


struct Fichero {
    int descriptor;
    int modo;
    char nombre[MAX];
};



void Cmd_hist(char *tr[], HistList *list, char *cmd, int *terminado, HistList *L, int *total);
void imprimirPrompt();
int TrocearCadena(char *cadena, char *trozos[]);
void leerEntrada(char *cmd, char *tr[], char *cadena);
void Cmd_help(char *tr[]);
void Cmd_authors(char *tr[]);
void Cmd_date(char *tr[], char *cmd);
void Cmd_infosys();
void Cmd_pid();
void Cmd_ppid();
void Cmd_cd(char *tr[]);
void Cmd_open(char *tr[], int *total);
void Cmd_close(char *tr[], int *total);
void Cmd_dup(char *tr[], int *total);


#endif
