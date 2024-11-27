#include "comandosP0.h"
#include "lista.h"

struct Fichero TablaFicheros[MAX];

void processEntry(char ** arr, HistList * hists, int * terminado, int * total, char * cmd);

void Cmd_hist(char *tr[], HistList *list, char *cmd, int *terminado, HistList *L, int *total) {
    if (tr[1] == NULL) {
        PrintHistList(list);
    } else if (tr[1][0] == '-') {
        int n = atoi(tr[1] + 1);
        if (n > 0) {
            PrintLastNCommands(list, n);
        } else {
            printf("Número inválido de comandos a mostrar.\n");
        }
    } else {
        int n = atoi(tr[1]);
        if (n > 0) {
            HistNode *command = GetCommandByNumber(list, n);
            if (command != NULL) {
                printf("Reejecutando comando:\t");
                for (int i = 0; i < MAX && command->trozos[i] != NULL; i++) {
                    printf(" %s\t", command->trozos[i]);
                }
                printf("\n");
                processEntry(command->trozos, L, terminado, total, cmd);
            } else {
                printf("No hay comando número %d en el historial.\n", n);
            }
        } else {
            printf("Número de comando inválido.\n");
        }
    }
}

void imprimirPrompt() {
    printf(":Shell~$ ");
}

int TrocearCadena(char *cadena, char *trozos[]) {
    int i = 1;
    if ((trozos[0] = strtok(cadena, " \n\t")) == NULL)
        return 0;
    while ((trozos[i] = strtok(NULL, " \n\t")) != NULL)
        i++;
    return i;
}

void leerEntrada(char *cmd, char *tr[], char *cadena) {
    fgets(cadena, MAX, stdin);
    strcpy(cmd, cadena);
    strtok(cmd, "\n");
    TrocearCadena(cadena, tr);
}

void Cmd_date(char *tr[], char *cmd) {
    time_t t;
    struct tm *info;

    // Obtener el tiempo actual
    time(&t);
    info = localtime(&t);

    // Verificar que tr[1] no sea NULL y luego compararlo
    if (tr[1] != NULL) {
        if (strcmp(tr[1], "-d") == 0) {
            printf("Current date: %02d/%02d/%d\n", info->tm_mday, info->tm_mon + 1, info->tm_year + 1900);
        } else if (strcmp(tr[1], "-t") == 0) {
            printf("Current time: %02d:%02d:%02d\n", info->tm_hour, info->tm_min, info->tm_sec);
        } else {
            // Imprimir mensaje de uso o manejo de parámetros desconocidos
            printf("Usage: %s [-d | -t]\n", cmd);
        }
    } else {
        // Si no se pasa ningún parámetro, imprimir fecha y hora
        printf("Current date: %02d/%02d/%d\n", info->tm_mday, info->tm_mon + 1, info->tm_year + 1900);
        printf("Current time: %02d:%02d:%02d\n", info->tm_hour, info->tm_min, info->tm_sec);
    }
}

void Cmd_infosys() {
#ifdef __linux__
    struct utsname info;
    if (uname(&info) != -1) {
        printf("System Information:\n");
        printf("Operating System: %s\n", info.sysname);
        printf("Node Name: %s\n", info.nodename);
        printf("Release: %s\n", info.release);
        printf("Version: %s\n", info.version);
        printf("Machine: %s\n", info.machine);
    } else {
        perror("uname");
    }
#else
    printf("System information is not available on Windows.\n");
#endif
}

void Cmd_pid() {
    printf("Pid de Shell: %d\n", getpid());
}

void Cmd_ppid() {
    printf("Pid del padre de la Shell: %d\n", getppid());
}

void Cmd_cd(char *tr[]) {
    if (tr[1] == NULL) {
        char path[MAX];
        if (getcwd(path, MAX) == NULL) {
            perror("getcwd");
        } else {
            printf("Directorio actual %s\n", path);
        }
    } else if (tr[2] != NULL) {
        printf("Invalid usage of chdir command.\n");
    } else {
        if (chdir(tr[1]) == 0) {
            printf("Changed directory to %s\n", tr[1]);
        } else {
            perror("chdir");
        }
    }
}
// Función para imprimir los modos de apertura de archivo
void OpeningMode(int mode) {
    if (mode & O_CREAT)
        printf("O_CREAT ");
    if (mode & O_EXCL)
        printf("O_EXCL ");
    if (mode & O_RDONLY)
        printf("O_RDONLY ");
    if (mode & O_WRONLY)
        printf("O_WRONLY ");
    if (mode & O_RDWR)
        printf("O_RDWR ");
    if (mode & O_APPEND)
        printf("O_APPEND ");
    if (mode & O_TRUNC)
        printf("O_TRUNC ");
    printf("\n");
}

// Función para ordenar la tabla de archivos por descriptor
void OrdenarTabla(int total) {
    if (total <= 1) return;  // No hay nada que ordenar si hay 0 o 1 archivo

    for (int i = 0; i < (total - 1); i++) {
        for (int j = i + 1; j < total; j++) {
            if (TablaFicheros[i].descriptor > TablaFicheros[j].descriptor) {
                struct Fichero temporal = TablaFicheros[i];
                TablaFicheros[i] = TablaFicheros[j];
                TablaFicheros[j] = temporal;
            }
        }
    }
}

// Función para listar los archivos abiertos
void ListOpenFiles(int df, const int *total) {
    if (df != -1) {
        printf("Descriptor: 0 -> entrada estandar O_RDWR\n");
        printf("Descriptor: 1 -> salida estandar O_RDWR\n");
        printf("Descriptor: 2 -> error estandar O_RDWR\n");
        OrdenarTabla(*total);
        for (int i = 0; i < *total; i++) {
            printf("Descriptor: %d -> %s ", TablaFicheros[i].descriptor, TablaFicheros[i].nombre);
            OpeningMode(TablaFicheros[i].modo);
        }
    } else {
        printf("No se puede listar los ficheros \n");
    }
}

// Función para añadir un archivo a la tabla de archivos abiertos
void AnadirFicherosAbiertos(int df, int mode, const char *nombre, int *total) {
    TablaFicheros[*total].descriptor = df;
    TablaFicheros[*total].modo = mode;
    strncpy(TablaFicheros[*total].nombre, nombre, sizeof(TablaFicheros[*total].nombre) - 1);
    TablaFicheros[*total].nombre[sizeof(TablaFicheros[*total].nombre) - 1] = '\0';
    printf("Añadida entrada %d a la tabla de ficheros abiertos \n", TablaFicheros[*total].descriptor);
    (*total)++;
}

// Función Cmd_open completada
void Cmd_open(char *tr[], int *total) {
    int i, df, mode = 0;

    if (tr[1] == NULL) { /* no hay parámetro */
        ListOpenFiles(0, total);
        return;
    }

    // Determinar los modos de apertura del archivo
    for (i = 2; tr[i] != NULL; i++) {
        if (!strcmp(tr[i], "cr")) mode |= O_CREAT;
        else if (!strcmp(tr[i], "ex")) mode |= O_EXCL;
        else if (!strcmp(tr[i], "ro")) mode |= O_RDONLY;
        else if (!strcmp(tr[i], "wo")) mode |= O_WRONLY;
        else if (!strcmp(tr[i], "rw")) mode |= O_RDWR;
        else if (!strcmp(tr[i], "ap")) mode |= O_APPEND;
        else if (!strcmp(tr[i], "tr")) mode |= O_TRUNC;
        else break;
    }

    // Abrir el archivo
    df = open(tr[1], mode, 0666);  // Permisos predeterminados para crear el archivo
    if (df == -1) {
        perror("Imposible abrir fichero");
    } else {
        AnadirFicherosAbiertos(df, mode, tr[1], total);
        printf("Añadida entrada a la tabla de ficheros abiertos\n");
    }
}

char *NombreFicheroDescriptor(int df) {
    return TablaFicheros[df - 1].nombre;  // Obtenemos el nombre del fichero a duplicar
}

void Cmd_dup(char *tr[], int *total) {
    int df, duplicado;
    char aux[MAX], *p;

    if (tr[1] == NULL || (df = atoi(tr[1])) < 0) {
        ListOpenFiles(0, total);
        return;
    }

    duplicado = dup(df);
    p = NombreFicheroDescriptor(*total);
    int mode = fcntl(duplicado, F_GETFL);
    sprintf(aux, "dup %d (%s)", df, p);
    AnadirFicherosAbiertos(duplicado, mode, aux, total);
}

void EliminarFicherosAbiertos(int df, int *total) {
    for (int i = 0; i < (*total); i++) {
        if (TablaFicheros[i].descriptor == df) {
            // Si el descriptor coincide con uno de la lista recorre desde esa posición
            // hasta el final para borrarlo y que los elementos posteriores retrocedan una posición
            printf("Eliminada la entrada %d de la tabla de ficheros abiertos \n", TablaFicheros[i].descriptor);
            for (int j = i; j < (*total) - 1; j++) {
                TablaFicheros[j] = TablaFicheros[j + 1];
            }
            (*total)--;
            break;
        }
    }
}

void Cmd_close(char *tr[], int *total) {
    int df;

    if (tr[1] == NULL || (df = atoi(tr[1])) < 0) { /* no hay parámetro */
        ListOpenFiles(0, total);
        return;
    }

    if (close(df) == -1)
        perror("Imposible cerrar descriptor");
    else
        EliminarFicherosAbiertos(df, total); // Sino elimina el fichero
}

