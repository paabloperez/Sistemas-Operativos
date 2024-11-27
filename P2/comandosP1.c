#include "comandosP1.h"

#include "comandosP0.h"

char LetraTF (mode_t m)
{
     switch (m&S_IFMT) { /*and bit a bit con los bits de formato,0170000 */
        case S_IFSOCK: return 's'; /*socket */
        case S_IFLNK: return 'l'; /*symbolic link*/
        case S_IFREG: return '-'; /* fichero normal*/
        case S_IFBLK: return 'b'; /*block device*/
        case S_IFDIR: return 'd'; /*directorio */
        case S_IFCHR: return 'c'; /*char device*/
        case S_IFIFO: return 'p'; /*pipe*/
        default: return '?'; /*desconocido, no deberia aparecer*/
     }
}
/*las siguientes funciones devuelven los permisos de un fichero en formato rwx----*/
/*a partir del campo st_mode de la estructura stat */
/*las tres son correctas pero usan distintas estrategias de asignaciÃ³n de memoria*/

char * ConvierteModo (mode_t m, char *permisos)
{
    strcpy (permisos,"---------- ");

    permisos[0]=LetraTF(m);
    if (m&S_IRUSR) permisos[1]='r';    /*propietario*/
    if (m&S_IWUSR) permisos[2]='w';
    if (m&S_IXUSR) permisos[3]='x';
    if (m&S_IRGRP) permisos[4]='r';    /*grupo*/
    if (m&S_IWGRP) permisos[5]='w';
    if (m&S_IXGRP) permisos[6]='x';
    if (m&S_IROTH) permisos[7]='r';    /*resto*/
    if (m&S_IWOTH) permisos[8]='w';
    if (m&S_IXOTH) permisos[9]='x';
    if (m&S_ISUID) permisos[3]='s';    /*setuid, setgid y stickybit*/
    if (m&S_ISGID) permisos[6]='s';
    if (m&S_ISVTX) permisos[9]='t';

    return permisos;
}


char * ConvierteModo2 (mode_t m)
{
    static char permisos[12];
    strcpy (permisos,"---------- ");

    permisos[0]=LetraTF(m);
    if (m&S_IRUSR) permisos[1]='r';    /*propietario*/
    if (m&S_IWUSR) permisos[2]='w';
    if (m&S_IXUSR) permisos[3]='x';
    if (m&S_IRGRP) permisos[4]='r';    /*grupo*/
    if (m&S_IWGRP) permisos[5]='w';
    if (m&S_IXGRP) permisos[6]='x';
    if (m&S_IROTH) permisos[7]='r';    /*resto*/
    if (m&S_IWOTH) permisos[8]='w';
    if (m&S_IXOTH) permisos[9]='x';
    if (m&S_ISUID) permisos[3]='s';    /*setuid, setgid y stickybit*/
    if (m&S_ISGID) permisos[6]='s';
    if (m&S_ISVTX) permisos[9]='t';

    return permisos;
}

char * ConvierteModo3 (mode_t m)
{
    char *permisos;

    if ((permisos=(char *) malloc (12))==NULL)
        return NULL;
    strcpy (permisos,"---------- ");

    permisos[0]=LetraTF(m);
    if (m&S_IRUSR) permisos[1]='r';    /*propietario*/
    if (m&S_IWUSR) permisos[2]='w';
    if (m&S_IXUSR) permisos[3]='x';
    if (m&S_IRGRP) permisos[4]='r';    /*grupo*/
    if (m&S_IWGRP) permisos[5]='w';
    if (m&S_IXGRP) permisos[6]='x';
    if (m&S_IROTH) permisos[7]='r';    /*resto*/
    if (m&S_IWOTH) permisos[8]='w';
    if (m&S_IXOTH) permisos[9]='x';
    if (m&S_ISUID) permisos[3]='s';    /*setuid, setgid y stickybit*/
    if (m&S_ISGID) permisos[6]='s';
    if (m&S_ISVTX) permisos[9]='t';

    return permisos;
}

void Cmd_cwd() { //Imprime el directorio actual de la Shell
    char path[MAX]; // Array para almacenar la ruta
    if (getcwd(path, MAX) == NULL) { // Obtiene el directorio de trabajo actual
        perror("getcwd"); // Si hay un error, imprime el mensaje de error
    } else {
        printf("Directorio actual %s\n", path); // Imprime el directorio actual
    }
}

int ContarTrozos(char *tr[]) {
    int count = 0;

    // Iteramos sobre el arreglo de cadenas hasta encontrar un NULL
    while (tr[count] != NULL) {
        count++;
    }
    return count;  // Retornamos el número total de trozos
}

void Cmd_makefile(char *tr[]) { //Crea un archivo
    struct stat st; //EStructura donde se almacenará la información del archivo

    if (tr[1] == NULL) { // Verifica si no se han pasado argumentos al comando
        Cmd_cwd();
    }else {
        const mode_t mode= 0775;
        if((lstat(tr[1], &st)==0)) printf("Imposible crear: El archivo '%s' ya existe\n",tr[1]); //Miramos si existe el archyivo
        else if(open(tr[1],O_CREAT,mode)==-1) perror("Imposible crear"); //SE crea el archivo
    }
}

void Cmd_makedir(char *tr[]) { //Crea un directorio
    struct stat st; //EStructura donde se almacenará la información del archivo

    if (tr[1] == NULL) { // Verifica si no se han pasado argumentos al comando
        Cmd_cwd();
    }else {
        const mode_t mode= 0775;
        if((lstat(tr[1], &st)==0)) printf("Imposible crear: El directorio '%s' ya existe\n",tr[1]); //Miramos si existe el archyivo
        else  if(mkdir(tr[1],mode)==-1) perror("Imposible crear"); //Crea el directorio
    }
}

void MostrarInfoArchivo(char *archivo,int long_format, int access_time,int show_links) { // Función auxiliar para mostrar la información de un archivo
    struct stat st;
    char permisos[12];
    struct passwd *owner;
    struct group *group;

    // Obtener información del archivo o directorio
    if (lstat(archivo, &st) == -1) {
        perror("lstat");
    }

    char date[20];
    strftime(date, sizeof(date), "%Y-%m-%d %H:%M", localtime(&st.st_mtime));
    owner = getpwuid(st.st_uid);
    group = getgrgid(st.st_gid);

    // Manejo de enlaces simbólicos
    char link_target[PATH_MAX] = "";
    if (S_ISLNK(st.st_mode) && show_links) {
        ssize_t len = readlink(archivo, link_target, sizeof(link_target) - 1);
        if (len != -1) {
            link_target[len] = '\0'; // Añadir terminador nulo
        } else {
            strcpy(link_target, "Error al leer el enlace"); // Manejo de error al leer el enlace
        }
    }
    if (long_format) {
        printf("| %-19s | %-8s | %-11s | %-12s | %-12s | %-10s | %-14s | %-19s |\n",
               "Fecha", "Nlinks", "Inode", "Propietario", "Grupo", "Permisos", "Tamaño", "Nombre");
    } else if (access_time) {
        printf("| %-19s | %-14s | %-19s |\n", "Fecha", "Tamaño", "Nombre");
    } else {
        printf("| %-14s | %-19s |\n", "Tamaño", "Nombre");
    }

    if (long_format) {
        printf("| %-19s | %-8ld | %-11ld | %-12s | %-12s | %-10s | %-14ld | %-19s",
               date, st.st_nlink, st.st_ino, owner->pw_name, group->gr_name,
               ConvierteModo(st.st_mode, permisos), (long)st.st_size, archivo);
        if (show_links && S_ISLNK(st.st_mode)) {
            printf(" -> %s", link_target);  // Mostrar destino del enlace simbólico
        }
        printf(" |\n");
    } else if (access_time) {
        printf("| %-19s | %-14ld | %-19s", date, st.st_size, archivo);
        if (show_links && S_ISLNK(st.st_mode)) {
            printf(" -> %s", link_target);  // Mostrar destino del enlace simbólico
        }
        printf(" |\n");
    } else {
        printf("| %-14ld | %-19s", st.st_size, archivo);
        if (show_links && S_ISLNK(st.st_mode)) {
            printf(" -> %s", link_target);  // Mostrar destino del enlace simbólico
        }
        printf(" |\n");
    }
}

// Función Cmd_listFile para listar la información de un único archivo
void Cmd_listFile(char *tr[]) {

    int  long_format = 0, access_time = 0, show_links = 0;
    int dir_start = 1, num_tr = ContarTrozos(tr);
    if(tr[1] == NULL) {
        Cmd_cwd();
    }else {
        // Procesar opciones
        for (int i = 1; i < num_tr; i++) {
            if (tr[i][0] == '-') {
                 if (strcmp(tr[i], "-long") == 0) {
                    long_format = 1;
                } else if (strcmp(tr[i], "-acc") == 0) {
                    access_time = 1;
                } else if (strcmp(tr[i], "-link") == 0) {
                    show_links = 1;
                }
            } else {
                dir_start = i;  // Primer argumento que no es opción
                break;
            }
        }

        if(tr[dir_start][0] == '-') {
            Cmd_cwd();
        }else {
            // Llamada a la función principal

            if(dir_start != num_tr) {
                for (int i = dir_start; i < num_tr; i++) {
                    MostrarInfoArchivo(tr[i], long_format, access_time, show_links);
                }
            }else {
                MostrarInfoArchivo(tr[dir_start], long_format, access_time, show_links);
            }
        }
    }
}


void listdir(char *tr[], int hid, int long_format, int access_time, int show_links, int dir_start) {
    DIR *dir;
    struct dirent *entry;
    struct stat st;
    char permisos[12];
    struct passwd *owner;
    struct group *group;

    // Abrir el directorio
    if ((dir = opendir(tr[dir_start])) == NULL) {
        printf("Imposible abrir el archivo\n");
        return;
    }

    // Imprimir encabezado
    printf("IMPRIMIENDO CONTENIDO DEL DIRECTORIO '%s' ***********************************************************************************************\n",tr[dir_start]);
    if (long_format) {
        printf("| %-19s | %-8s | %-11s | %-12s | %-12s | %-10s | %-14s | %-19s |\n",
               "Fecha", "Nlinks", "Inode", "Propietario", "Grupo", "Permisos", "Tamaño", "Nombre");
    } else if (access_time) {
        printf("| %-19s | %-14s | %-19s |\n", "Fecha", "Tamaño", "Nombre");
    } else {
        printf("| %-14s | %-19s |\n", "Tamaño", "Nombre");
    }

    // Leer el contenido del directorio
    while ((entry = readdir(dir)) != NULL) {
        // Controlar si se deben mostrar archivos ocultos
        if (!hid && entry->d_name[0] == '.') {
            continue;
        }

        // Construir la ruta completa del archivo
        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", tr[dir_start], entry->d_name);

        // Obtener información del archivo o directorio
        if (lstat(full_path, &st) == -1) {
            perror("lstat");
        }

        char date[20];
        strftime(date, sizeof(date), "%Y-%m-%d %H:%M", localtime(&st.st_mtime));
        owner = getpwuid(st.st_uid);
        group = getgrgid(st.st_gid);

        // Manejo de enlaces simbólicos
        char link_target[PATH_MAX] = "";
        if (S_ISLNK(st.st_mode) && show_links) {
            ssize_t len = readlink(full_path, link_target, sizeof(link_target) - 1);
            if (len != -1) {
                link_target[len] = '\0'; // Añadir terminador nulo
            }
        }

        // Imprimir información según el formato seleccionado
        if (long_format) {
            printf("| %-19s | %-8ld | %-11ld | %-12s | %-12s | %-10s | %-14ld | %-19s",
                   date, st.st_nlink, st.st_ino, owner->pw_name, group->gr_name,
                   ConvierteModo(st.st_mode, permisos), (long)st.st_size, entry->d_name);
            if (show_links && S_ISLNK(st.st_mode)) {
                printf(" -> %s", link_target);  // Mostrar destino del enlace simbólico
            }
            printf(" |\n");
        } else if (access_time) {
            printf("| %-19s | %-14ld | %-19s ", date, st.st_size, entry->d_name);
            if (show_links && S_ISLNK(st.st_mode)) {
                printf(" -> %s", link_target);  // Mostrar destino del enlace simbólico
            }
            printf(" |\n");
        } else {
            printf("| %-14ld | %-19s", st.st_size, entry->d_name);
            if (show_links && S_ISLNK(st.st_mode)) {
                printf(" -> %s", link_target);  // Mostrar destino del enlace simbólico
            }
            printf(" |\n");
        }
    }

    closedir(dir);
}

void Cmd_listdir(char *tr[]) {
    int include_hidden = 0, long_format = 0, access_time = 0, show_links = 0;
    int dir_start = 1, num_tr = ContarTrozos(tr);
    if(tr[1] == NULL) {
        Cmd_cwd();
    }else {
        // Procesar opciones
        for (int i = 1; i < num_tr; i++) {
            if (tr[i][0] == '-') {
                if (strcmp(tr[i], "-hid") == 0) {
                    include_hidden = 1;
                } else if (strcmp(tr[i], "-long") == 0) {
                    long_format = 1;
                } else if (strcmp(tr[i], "-acc") == 0) {
                    access_time = 1;
                } else if (strcmp(tr[i], "-link") == 0) {
                    show_links = 1;
                }
            } else {
                dir_start = i;  // Primer argumento que no es opción
                break;
            }
        }

        if(tr[dir_start][0] == '-') {
            Cmd_cwd();
        }else {
            // Llamada a la función principal

            if(dir_start != num_tr) {
                for (int i = dir_start; i < num_tr; i++) {
                    listdir(tr, include_hidden, long_format, access_time, show_links,i);
                }
            }else {
                listdir(tr, include_hidden, long_format, access_time, show_links, dir_start);
            }
        }
    }

}


void aux(char *tr, int hid, int long_format, int access_time, int show_links) {
    DIR *dir;
    struct dirent *entry;
    struct stat st;
    char permisos[12];
    struct passwd *owner;
    struct group *group;

    // Abrir el directorio
    if ((dir = opendir(tr)) == NULL) {
        printf("Imposible abrir el directorio: %s\n", tr);
        return;
    }

    // Imprimir encabezado
    printf("IMPRIMIENDO CONTENIDO DEL DIRECTORIO '%s' ***********************************************************************************************\n", tr);
    if (long_format) {
        printf("| %-19s | %-8s | %-11s | %-12s | %-12s | %-10s | %-14s | %-19s |\n",
               "Fecha", "Nlinks", "Inode", "Propietario", "Grupo", "Permisos", "Tamaño", "Nombre");
    } else if (access_time) {
        printf("| %-19s | %-14s | %-19s |\n", "Fecha", "Tamaño", "Nombre");
    } else {
        printf("| %-14s | %-19s |\n", "Tamaño", "Nombre");
    }

    // Leer el contenido del directorio
    while ((entry = readdir(dir)) != NULL) {
        // Controlar si se deben mostrar archivos ocultos
        if (!hid && entry->d_name[0] == '.') {
            continue;
        }

        // Construir la ruta completa del archivo
        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", tr, entry->d_name);

        // Obtener información del archivo o directorio
        if (lstat(full_path, &st) == -1) {
            perror("lstat");
            continue; // Continuar en caso de error
        }

        char date[20];
        strftime(date, sizeof(date), "%Y-%m-%d %H:%M", localtime(&st.st_mtime));
        owner = getpwuid(st.st_uid);
        group = getgrgid(st.st_gid);

        // Manejo de enlaces simbólicos
        char link_target[PATH_MAX] = "";
        if (S_ISLNK(st.st_mode) && show_links) {
            ssize_t len = readlink(full_path, link_target, sizeof(link_target) - 1);
            if (len != -1) {
                link_target[len] = '\0'; // Añadir terminador nulo
            }
        }

        // Imprimir información según el formato seleccionado
        if (long_format) {
            printf("| %-19s | %-8ld | %-11ld | %-12s | %-12s | %-10s | %-14ld | %-19s",
                   date, st.st_nlink, st.st_ino, owner->pw_name, group->gr_name,
                   ConvierteModo(st.st_mode, permisos), (long)st.st_size, entry->d_name);
            if (show_links && S_ISLNK(st.st_mode)) {
                printf(" -> %s", link_target);  // Mostrar destino del enlace simbólico
            }
            printf(" |\n");
        } else if (access_time) {
            printf("| %-19s | %-14ld | %-19s ", date, st.st_size, entry->d_name);
            if (show_links && S_ISLNK(st.st_mode)) {
                printf(" -> %s", link_target);  // Mostrar destino del enlace simbólico
            }
            printf(" |\n");
        } else {
            printf("| %-14ld | %-19s", st.st_size, entry->d_name);
            if (show_links && S_ISLNK(st.st_mode)) {
                printf(" -> %s", link_target);  // Mostrar destino del enlace simbólico
            }
            printf(" |\n");
        }
    }

    closedir(dir);
}

void reclist(char *tr[], int hid, int long_format, int access_time, int show_links, char *direc, bool prim) {
    DIR *dir;
    struct dirent *entry;
    struct stat st;
    char permisos[12];


    // Abrir el directorio
    if ((dir = opendir(direc)) == NULL) {
        printf("Imposible abrir el directorio: %s\n", direc);
        return;
    }

    if(prim) { //Si es la primera iteración mostramos el contenido de la carpeta inicial
       aux(direc, hid, long_format, access_time, show_links);
    }

    // Leer el contenido del directorio
    while ((entry = readdir(dir)) != NULL) {
        // Construir la ruta completa del archivo
        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", direc, entry->d_name);

        // Obtener información del archivo o directorio
        if (lstat(full_path, &st) == -1) {
            perror("lstat");
            continue; // Continuar en caso de error
        }

        ConvierteModo(st.st_mode, permisos);

        // Controlar si se deben mostrar archivos ocultos
        if (entry->d_name[0] != '.') {
            if (permisos[0] == 'd') {
                aux(full_path, hid, long_format, access_time, show_links); // Llamada a aux para imprimir info
                reclist(tr, hid, long_format, access_time, show_links, full_path,prim=false); // Llamada recursiva
            }
        }
    }
    closedir(dir);
}

void Cmd_reclist(char *tr[]) { // Lista los directorios de manera recursiva
    int include_hidden = 0, long_format = 0, access_time = 0, show_links = 0;
    int dir_start = 1, num_tr = ContarTrozos(tr);
    bool prim = true;
    if (tr[1] == NULL) {
        Cmd_cwd();
    } else {
        // Procesar opciones
        for (int i = 1; i < num_tr; i++) {
            if (tr[i][0] == '-') {
                if (strcmp(tr[i], "-hid") == 0) {
                    include_hidden = 1;
                } else if (strcmp(tr[i], "-long") == 0) {
                    long_format = 1;
                } else if (strcmp(tr[i], "-acc") == 0) {
                    access_time = 1;
                } else if (strcmp(tr[i], "-link") == 0) {
                    show_links = 1;
                }
            } else {
                dir_start = i;  // Primer argumento que no es opción
                break;
            }
        }

        // Asegurarse de que el primer argumento no sea una opción
        if (tr[dir_start][0] == '-') {
            Cmd_cwd();
        } else {
            // Llamada a la función principal
            for (int i = dir_start; i < num_tr; i++) {
                reclist(tr, include_hidden, long_format, access_time, show_links, tr[i],prim);
            }
        }
    }
}

void revlist(char *tr[], int hid, int long_format, int access_time, int show_links, char *direc, bool prim) {
    DIR *dir;
    struct dirent *entry;
    struct stat st;
    char permisos[12];


    // Abrir el directorio
    if ((dir = opendir(direc)) == NULL) {
        printf("Imposible abrir el directorio: %s\n", direc);
        return;
    }


    // Leer el contenido del directorio
    while ((entry = readdir(dir)) != NULL) {
        // Construir la ruta completa del archivo
        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", direc, entry->d_name);

        // Obtener información del archivo o directorio
        if (lstat(full_path, &st) == -1) {
            perror("lstat");
            continue; // Continuar en caso de error
        }

        ConvierteModo(st.st_mode, permisos);

        // Controlar si se deben mostrar archivos ocultos
        if (entry->d_name[0] != '.') {
            if (permisos[0] == 'd') {

                reclist(tr, hid, long_format, access_time, show_links, full_path,prim); // Llamada recursiva
                aux(full_path, hid, long_format, access_time, show_links); // Llamada a aux para imprimir info
            }
        }
    }
    prim=true;
    if(prim) { //Si es la ultima iteración mostramos el contenido de la carpeta inicial
        aux(direc, hid, long_format, access_time, show_links);
    }

    closedir(dir);
}

void Cmd_revlist(char *tr[]) { //LIsta los directorios de manera recursiva (Subdirectorios despues)

    int include_hidden = 0, long_format = 0, access_time = 0, show_links = 0;
    int dir_start = 1, num_tr = ContarTrozos(tr);
    bool prim = false;
    if (tr[1] == NULL) {
        Cmd_cwd();
    } else {
        // Procesar opciones
        for (int i = 1; i < num_tr; i++) {
            if (tr[i][0] == '-') {
                if (strcmp(tr[i], "-hid") == 0) {
                    include_hidden = 1;
                } else if (strcmp(tr[i], "-long") == 0) {
                    long_format = 1;
                } else if (strcmp(tr[i], "-acc") == 0) {
                    access_time = 1;
                } else if (strcmp(tr[i], "-link") == 0) {
                    show_links = 1;
                }
            } else {
                dir_start = i;  // Primer argumento que no es opción
                break;
            }
        }

        // Asegurarse de que el primer argumento no sea una opción
        if (tr[dir_start][0] == '-') {
            Cmd_cwd();
        } else {
            // Llamada a la función principal
            for (int i = dir_start; i < num_tr; i++) {
                revlist(tr, include_hidden, long_format, access_time, show_links, tr[i],prim);
            }
        }
    }

}
bool Esdirectoriovacio(char *tr[],int i) { //FUnción auxiliar para saber si un directorio esta vacío o no
    DIR *dir;
    struct dirent *entry;

    // Abrir el directorio
    if ((dir = opendir(tr[i])) == NULL) {
        perror("Error al abrir el directorio");
        return false;  // Retorna false en caso de error al abrir
    }

    // Leer las entradas del directorio
    while ((entry = readdir(dir)) != NULL) {
        // Ignorar '.' y '..'
        if (entry->d_name[0] != '.') {
            closedir(dir);
            return false;  // El directorio no está vacío
        }
    }

    // Cerrar el directorio
    closedir(dir);
    return true;  // El directorio está vacío
}

void Cmd_erase(char *tr[]){ // erase [name1 name2 ..]	Borra ficheros o directorios vacios
    struct stat st;
    if (tr[1] == NULL) {
        Cmd_cwd();
    } else {
    for (int i = 1; i < ContarTrozos(tr); i++) {

            // Obtener information del archivo o directorio
            if (lstat(tr[i], &st) == -1) {
                perror("Imposible borrar");
                return;
            }

            //Miramos si lo que nos pasan es un fichero (-) o un directorio (d)
            char TipoDeArchivo = LetraTF(st.st_mode);

            if (TipoDeArchivo == '-') {//Si es un fichero

                if(remove(tr[i]) == -1) perror("Imposible borrar"); //Borramos el archivo
                printf("Fichero '%s' borrado con éxito \n",tr[i]);

            } else if (TipoDeArchivo == 'd') {//Si es un directorio

                bool directorio_vacio = Esdirectoriovacio(tr,i); //Comprobamos si esta vacío el directorio

                if(directorio_vacio) {
                    if(remove(tr[i]) == -1) perror("Imposible borrar"); //Borramos el archivo
                    printf("Directorio '%s' borrado con éxito \n",tr[i]);
                }else {
                    printf("Imposible borrar: El directorio '%s' no está vacío\n", tr[i]);
                }

            }
        }
    }
}
void borrar_recursivo(char *nombreArchivo) {
    DIR *dir;
    struct dirent *entry;
    struct stat st;
    char path[1024];  // Para almacenar la ruta completa de los archivos y directorios

    // Abrir el directorio
    if ((dir = opendir(nombreArchivo)) == NULL) {
        perror("Imposible borrar");
        return;
    }

    // Leer el contenido del directorio
    while ((entry = readdir(dir)) != NULL) {
        // Ignorar los directorios "." y ".."
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {

            // Construye de manera segura (evitando desbordamientos del bufer) la ruta completa combinando el directorio y el nombre del archivo
            snprintf(path, sizeof(path), "%s/%s", nombreArchivo, entry->d_name);


            // Obtener información del archivo o directorio
            if (lstat(path, &st) == -1) {
                perror("Error al obtener información");
            } else {
                //Miramos si lo que nos pasan es un fichero (-) o un directorio (d)
                char TipoDeArchivo = LetraTF(st.st_mode);

                // Si es un directorio, llamamos a la función recursivamente
                if (TipoDeArchivo == 'd') {
                    borrar_recursivo(path);  // Llamada recursiva
                } else {
                    // Si es un archivo regular, lo eliminamos
                    if (remove(path) == -1) {
                        perror("Imposible borrar archivo");
                    } else {
                        printf("Archivo '%s' borrado con éxito\n", path);
                    }
                }
            }
        }
    }

    // Cerrar el directorio
    closedir(dir);

    // Intentar eliminar el directorio una vez que esté vacío
    if (rmdir(nombreArchivo) == -1) {
        perror("Imposible borrar directorio");
    } else {
        printf("Directorio '%s' borrado con éxito\n", nombreArchivo);
    }
}

void Cmd_delrec(char *tr[]) {
    struct stat st;

    if (tr[1] == NULL) {
        Cmd_cwd();
        return;
    }

    for (int i = 1; i < ContarTrozos(tr); i++) {
        // Obtener información del archivo o directorio
        if (lstat(tr[i], &st) == -1) {
            perror("Imposible obtener información");
            return;
        }

        //Miramos si lo que nos pasan es un fichero (-) o un directorio (d)
        char TipoDeArchivo = LetraTF(st.st_mode);

        if (TipoDeArchivo == '-') {//Si es un fichero
            if (remove(tr[i]) == -1) {
                perror("Imposible borrar archivo");
            } else {
                printf("Fichero '%s' borrado con éxito\n", tr[i]);
            }
        } else if (TipoDeArchivo == 'd') {//Si es un directorio, llamamos a la funcion auxiliar
            borrar_recursivo(tr[i]);
        }
    }
}


