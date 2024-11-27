#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include "comandosP0.h"
#include "comandosP1.h"
#include "lista.h"

void processEntry(char *tr[], HistList *L, int *terminado, int *total, char *cmd) {

    if (tr[0] != NULL) {
        if (strcmp(tr[0], "authors") == 0) {
            Cmd_authors(tr);
        } else if (strcmp(tr[0], "pid") == 0) {
            Cmd_pid();
        } else if (strcmp(tr[0], "makefile") == 0) {
            Cmd_makefile(tr);
        } else if (strcmp(tr[0], "makedir") == 0) {
            Cmd_makedir(tr);
        } else if (strcmp(tr[0], "cwd") == 0) {
            Cmd_cwd();
        } else if (strcmp(tr[0], "listfile") == 0) {
            Cmd_listFile(tr);
        } else if (strcmp(tr[0], "listdir") == 0) {
            Cmd_listdir(tr);
        } else if (strcmp(tr[0], "erase") == 0) {
            Cmd_erase(tr);
        } else if (strcmp(tr[0], "delrec") == 0) {
            Cmd_delrec(tr);
        } else if (strcmp(tr[0], "reclist") == 0) {
            Cmd_reclist(tr);
        } else if (strcmp(tr[0], "revlist") == 0) {
            Cmd_revlist(tr);
        } else if (strcmp(tr[0], "ppid") == 0) {
            Cmd_ppid();
        } else if (strcmp(tr[0], "cd") == 0) {
            Cmd_cd(tr);
        } else if (strcmp(tr[0], "date") == 0) {
            Cmd_date(tr, cmd);
        } else if (strcmp(tr[0], "historic") == 0) {
            Cmd_hist(tr, L, cmd, terminado, L, total);
        } else if (strcmp(tr[0], "open") == 0) {
            Cmd_open(tr, total);
        } else if (strcmp(tr[0], "close") == 0) {
            Cmd_close(tr, total);
        } else if (strcmp(tr[0], "dup") == 0) {
            Cmd_dup(tr, total);
        } else if (strcmp(tr[0], "infosys") == 0) {
            Cmd_infosys();
        } else if (strcmp(tr[0], "help") == 0) {
            Cmd_help(tr);
        } else if (strcmp(tr[0], "quit") == 0 || strcmp(tr[0], "bye") == 0 || strcmp(tr[0], "exit") == 0) {
            exit(0);
        } else {
            fprintf(stderr, "%s '%s'\n", strerror(3), cmd);
        }
    }
}

void Cmd_help(char *tr[]) {
    if (tr[1] == NULL) {
        printf("Available commands: authors, pid, ppid, cd, date, historic, open, close, dup, infosys, help, quit, exit, bye.\n");
    } else if (strcmp(tr[1], "authors") == 0) {
        printf("authors: Prints the names and logins of the program authors.\n "
               "* authors -l: Prints the logins of the program authors.\n"
               "* authors -n: Prints the names of the program authors.\n");
    } else if (strcmp(tr[1], "pid") == 0) {
        printf("Prints the pid of the process executing the shell.\n");
    } else if (strcmp(tr[1], "ppid") == 0) {
        printf("Prints the pid of the shell's parent process.\n");
    } else if (strcmp(tr[1], "cd") == 0) {
        printf("cd [dir]: Changes the current working directory of the shell to dir.\n");
    } else if (strcmp(tr[1], "date") == 0) {
        printf("date: Prints the current date in the format DD/MM/YYYY and the current time in the format hh:mm:ss.\n"
               "* date -t: Prints the current time in the format hh:mm:ss.\n"
               "* date -d: Prints the current date in the format DD/MM/YYYY.\n");
    } else if (strcmp(tr[1], "historic") == 0) {
        printf("historic: Shows the historic of commands executed by the shell.\n"
               "* historic N: Repeats command number N (from historic list)\n"
               "* historic -N Prints only the lastN comands\n");
    } else if (strcmp(tr[1], "open") == 0) {
        printf("open [file] mode: Opens a file and adds it.\n"
               "The mode we’ll use cr for O CREAT, ap for O APPEND, ex for O EXCL, ro for O RDONLY,"
               "rw for O RDWR, wo for O WRONLY and tr for O TRUNC.\n");
    } else if (strcmp(tr[1], "close") == 0) {
        printf("close [df] : Closes the df file descriptor and eliminates the corresponding item from the list.\n");
    } else if (strcmp(tr[1], "dup") == 0) {
        printf("dup [df] : Duplicates the df file descriptor.\n");
    } else if (strcmp(tr[1], "infosys") == 0) {
        printf("Prints information on the machine running the shell.\n");
    } else if (strcmp(tr[1], "help") == 0) {
        printf("Help displays a list of available commands.\n");
    } else if (strcmp(tr[1], "quit") == 0 || strcmp(tr[1], "bye") == 0 || strcmp(tr[1], "exit") == 0) {
        printf("Ends the shell.\n");
    }else if (strcmp(tr[1], "makefile") == 0) {
        printf("makefile : creates a file\n");
    }else if (strcmp(tr[1], "makedir") == 0) {
        printf("makefile : creates a directory\n");
    }else if (strcmp(tr[1], "listfile") == 0) {
        printf("listfile [-long][-link][-acc] name1 name2 ..	lista ficheros;\n  -hid : muestra lor archivos ocultos\n -long: listado largo\n -acc: acesstime\n -link: si es enlace simbolico, el path contenido\n");
    } else if (strcmp(tr[1], "cwd") == 0) {
        printf("cwd 	Muestra el directorio actual del shell\n");
    }else if (strcmp(tr[1], "listdir") == 0) {
        printf("listdir[-long][-link][-acc][-hid] name1 name2 ..	lista contenido de un directorio;\n"
        "-long: listado largo\n"
        "-acc: acesstime\n"
        "-link: si es enlace simbolico, el path contenido.\n");
    }else if (strcmp(tr[1], "reclist") == 0) {
        printf("Reclist [-hid][-long][-link][-acc] n1 n2 ..	lista recursivamente contenidos de directorios (subdirs despues)\n"
        "-long: listado largo\n"
        "-acc: acesstime\n"
        "-link: si es enlace simbolico, el path contenido\n");
    }else if (strcmp(tr[1], "revlist") == 0) {
        printf("Reclist [-hid][-long][-link][-acc] n1 n2 ..	lista recursivamente contenidos de directorios (subdirs antes)\n"
        "-long: listado largo\n"
        "-acc: acesstime.\n"
        "-link: si es enlace simbolico, el path contenido\n");
    }else if (strcmp(tr[1], "erase") == 0) {
        printf("erase [name1 name2 ..]	Borra ficheros o directorios vacios\n");
    }else if (strcmp(tr[1], "delrec") == 0) {
        printf("erase [name1 name2 ..]	Borra ficheros o directorios no vacíos de manera recursiva\n");
    }
}

void Cmd_authors(char *tr[]) {
    if (tr[1] == NULL) {
        printf("Pablo Pérez Noceda: pablo.perez5@udc.es\nDiego Losada Gomez: diego.lgomez@udc.es\n");
    } else if (strcmp(tr[1], "-l") == 0) {
        printf("pablo.perez5@udc.es\n diego.lgomez@udc.es\n");
    } else if (strcmp(tr[1], "-n") == 0) {
        printf("Pablo Perez Noceda\nDiego Losada Gomez\n");
    }
}

int main(void) {
    HistList history;
    CreateHistList(&history);

    int total = 0;
    int running = 1;
    char cmd[MAX];
    char *tokens[MAX_CMD];

    while (running) {
        imprimirPrompt();
        leerEntrada(cmd, tokens, cmd);
        InsertHistElement(&history, tokens);
        processEntry(tokens, &history, &running, &total, cmd);
    }

    // Liberar memoria del historial
    FreeHistList(&history);
}
