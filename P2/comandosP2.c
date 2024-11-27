#include "comandosP2.h"    // Define las funciones específicas de tu programa, como estructuras y comandos personalizados.
#include "listaMemoriaP2.h" // Manejo de la lista de bloques de memoria compartida o mapeada.

#include <errno.h>         // Para la gestión de errores (`errno` y `perror`).
#include <fcntl.h>         // Para control de archivos (e.g., `open`, flags como `O_RDONLY`, `O_RDWR`).
#include <stdint.h>
#include <stdio.h>         // Entrada/salida estándar (e.g., `printf`, `perror`).
#include <stdlib.h>        // Funciones estándar de memoria y conversión (e.g., `malloc`, `strtol`, `exit`).
#include <string.h>        // Manipulación de cadenas (e.g., `strchr`, `strdup`).
#include <time.h>          // Para trabajar con tiempos (e.g., `struct tm`, `time`).
#include <unistd.h>        // Para funciones del sistema UNIX (e.g., `fork`, `execvp`, `getpid`).
#include <sys/mman.h>      // Para funciones de memoria mapeada (`mmap`, `munmap`).
#include <sys/stat.h>      // Para información de ficheros (e.g., `stat`).
#include <sys/ipc.h>       // Claves de IPC (memoria compartida, semáforos, colas).
#include <sys/shm.h>       // Funciones de memoria compartida (`shmget`, `shmat`, `shmctl`).
#include <sys/wait.h>      // Para `waitpid` (gestión de procesos hijo).

void recurse (int n) {
    char automatico[TAMANHO]; //Declaramos o array atomático
    static char estatico[TAMANHO]; //Declaracion do array estático

    printf("parámetro: %i (%p) array %p, arr estatico %p \n",n,&n, automatico, estatico);

    if (n>0) {
        recurse(n-1);
    }

}

void Cmd_recurse(char *tr[]) {
    /*
     * A función recurse(n) executa unha chamada recursiva n veces. Dentro da función,
     * créanse dous arrays: un array automático e un array estático, ambos de tamaño 2048 bytes.
     * Antes de cada chamada recursiva, imprime os enderezos de memoria dos dous arrays,
     * o valor do parámetro, e o número de recursión actual. Isto permite observar os cambios
     * nos enderezos e o comportamento da recursión en cada chamada.
     */

    if(tr[1]!=NULL) {
        recurse(atoi(tr[1]));
    }
}


void malloc_aux(char *tr[], char *accion, tListaMemoria *listaMemoria) {
    void *p;
    struct tm *fecha;
    time_t aux_time = time(0);
    tElementoMemoria elemento;  // Inicializa la estructura directamente
    int posicion;

    if (tr[2] != NULL) {
        size_t size = atoi(tr[2]);

        if (strcmp(accion, "malloc") == 0) {
            if (size == 0) {
                printf("ERROR: No se asignan bloques de 0 bytes\n");
            } else {
                p = malloc(size);

                if (p != NULL) {
                    // Obtener la fecha actual
                    fecha = localtime(&aux_time);

                    // Inicializar los campos de la estructura tElementoMemoria
                    elemento.tipo = MALLOC;
                    elemento.direccion = p;
                    elemento.tamaño = size;
                    strftime(elemento.fecha, 128, "%d %b %H:%M", fecha);

                    // Insertar el elemento en la lista
                    if (insertItemM(elemento, listaMemoria)) {
                        printf("Asignados %lu bytes en %p\n", size, p);
                    } else {
                        perror("malloc falló");
                    }
                } else {
                    perror("malloc falló");
                }
            }
        } else if (strcmp(accion, "free") == 0) {
            if (size == 0) {
                printf("ERROR: No se borran bloques de 0 bytes\n");
            } else {
                for(posicion = firstM(*listaMemoria); posicion <= lastM(*listaMemoria); posicion++) {
                    elemento = getItemM(posicion, *listaMemoria);

                    if (elemento.tamaño == size && elemento.direccion != NULL && elemento.tipo == MALLOC) {
                        printf("Borrado con exito el bloque malloc de tamaño %lu en la direccion %p\n",elemento.tamaño,elemento.direccion);
                        deleteItemM(posicion,listaMemoria);
                        free(elemento.direccion);
                        break;
                    } else if (elemento.tamaño == size && elemento.direccion == NULL) {
                        printf("ERROR: Dirección NULL encontrada.\n");
                        return;
                    }
                }
            }
        }
    } else {
        // Imprimir la lista de bloques asignados
        imprimir_malloc(*listaMemoria);
    }
}

//Trozo de códio proporcionado por los profes
void * MapearFichero (char * fichero, int protection)
{
    int df, map=MAP_PRIVATE,modo=O_RDONLY;
    struct stat s;
    void *p;

    if (protection&PROT_WRITE)
        modo=O_RDWR;
    if (stat(fichero,&s)==-1 || (df=open(fichero, modo))==-1)
        return NULL;
    if ((p=mmap (NULL,s.st_size, protection,map,df,0))==MAP_FAILED)
        return NULL;
    /* Guardar en la lista    InsertarNodoMmap (&L,p, s.st_size,df,fichero); */
    /* Gurdas en la lista de descriptores usados df, fichero*/
    return p;
}

void do_AllocateMmap(char *arg[],tListaMemoria *listaMemoria)
{
    char *perm;
    void *p;
    int protection=0;
    tElementoMemoria elemento;
    struct tm *fecha;
    time_t aux_time = time(0);
    struct stat archivo;
    stat(arg[2], &archivo);

    //Verificamos si el archivo existe
    if (stat(arg[2], &archivo) == -1) {
        perror("ERROR al obtener información del archivo\n");
        return;
    }

    if ((perm=arg[3])!=NULL && strlen(perm)<4) {
        if (strchr(perm,'r')!=NULL) protection|=PROT_READ;
        if (strchr(perm,'w')!=NULL) protection|=PROT_WRITE;
        if (strchr(perm,'x')!=NULL) protection|=PROT_EXEC;
    }
    if ((p=MapearFichero(arg[2],protection))==NULL)
        perror ("Imposible mapear fichero");
    else {
        fecha = localtime(&aux_time);

        elemento.tipo = MMAP;
        elemento.direccion = p;
        elemento.tamaño = archivo.st_size;
        strftime(elemento.fecha, 128, "%d %b %H:%M", fecha);
        strcpy(elemento.informacion.archivo.nombre,arg[2]);

        // Abrir el archivo para obtener su descriptor de archivo
        // Usamos O_RDONLY ya que solo queremos leer el archivo
        elemento.informacion.archivo.descriptor = open(arg[2], O_CREAT,O_RDONLY);

        // Verificar si el descriptor de archivo se ha abierto correctamente
        if (elemento.informacion.archivo.descriptor == -1) {
            perror("ERROR al abrir el archivo\n");
            return;
        }
        if(insertItemM(elemento,listaMemoria)) {
            printf ("fichero %s mapeado en %p\n", arg[2], p);
        }else {
            printf("ERROR: No se ha podido mapear el archivo indicadoi\n");
        }
    }
}

void mmap_aux(char *tr[], char *accion, tListaMemoria *listaMemoria) {
    if (tr[2] != NULL) {
        if (strcmp(accion, "map") == 0) {
            do_AllocateMmap(tr, listaMemoria);
        } else if (strcmp(accion, "unmap") == 0) {
            int pos;
            tElementoMemoria elemento;
            bool encontrado = false;

            for (pos = firstM(*listaMemoria); pos <= lastM(*listaMemoria); pos++) {
                elemento = getItemM(pos, *listaMemoria);

                // Verificamos si es del tipo MMAP y el nombre coincide
                if (elemento.tipo == MMAP && strcmp(elemento.informacion.archivo.nombre, tr[2]) == 0) {
                    encontrado = true;

                    // Desmapeamos la región de memoria
                    if (munmap(elemento.direccion, elemento.tamaño) == -1) {
                        perror("Error al desmapear el archivo");
                    } else {
                        printf("Archivo %s desmapeado correctamente de %p\n",
                               elemento.informacion.archivo.nombre, elemento.direccion);
                    }

                    // Cerramos el descriptor de archivo
                    close(elemento.informacion.archivo.descriptor);

                    // Eliminamos el elemento de la lista
                    deleteItemM(pos, listaMemoria);
                    break;
                }
            }

            if (!encontrado) {
                printf("No se encontró un archivo mapeado con el nombre %s\n", tr[2]);
            }
        }
    } else {
        // Imprimimos la lista de archivos mapeados
        imprimir_mmap(*listaMemoria);
    }
}
void *ObtenerMemoriaShmget(key_t clave, size_t tam, tListaMemoria *listaMemoria) {
    void *p;
    int aux, id, flags = 0777;
    struct shmid_ds s;

    tElementoMemoria elemento;
    struct tm *fecha;
    time_t aux_time = time(0);

    if (tam)
        flags |= IPC_CREAT | IPC_EXCL;

    if (clave == IPC_PRIVATE) {
        fprintf(stderr, "ERROR: La clave IPC_PRIVATE no es válida para memoria compartida.\n");
        errno = EINVAL;
        return NULL;
    }

    if ((id = shmget(clave, tam, flags)) == -1) {
        perror("ERROR al obtener el segmento de memoria compartida (shmget)");
        return NULL;
    }
    printf("Segmento de memoria compartida obtenido con id: %d\n", id);

    if ((p = shmat(id, NULL, 0)) == (void *)-1) {
        aux = errno;
        if (tam)
            shmctl(id, IPC_RMID, NULL);
        errno = aux;
        perror("ERROR al adjuntar el segmento de memoria compartida (shmat)");
        return NULL;
    }
    printf("Segmento de memoria compartida adjuntado en la dirección: %p\n", p);

    if (shmctl(id, IPC_STAT, &s) == 0) {
        fecha = localtime(&aux_time);
        elemento.tipo = SHARED;
        elemento.direccion = p;
        strftime(elemento.fecha, 128, "%d %b %H:%M", fecha);
        elemento.informacion.clave = clave;
        elemento.tamaño = s.shm_segsz;

        if (listaMemoria == NULL) {
            fprintf(stderr, "ERROR: La lista de memoria no está inicializada.\n");
            return NULL;
        }

        if (!insertItemM(elemento, listaMemoria)) {
            fprintf(stderr, "ERROR: Fallo al insertar el elemento en la lista de memoria.\n");
        } else {
            printf("Elemento insertado correctamente en la lista de memoria.\n");
        }
    } else {
        perror("ERROR al obtener información del segmento de memoria (shmctl)");
    }
    return p;
}

void do_AllocateCreateshared (char *tr[],tListaMemoria *lista_memoria){
    key_t cl;
    size_t tam;
    void *p;

    if (tr[2]==NULL || tr[3]==NULL) {
        imprimir_shared(*lista_memoria);
        return;
    }

    cl=(key_t)  strtoul(tr[2],NULL,10);
    tam=(size_t) strtoul(tr[3],NULL,10);
    if (tam==0) {
        printf ("No se asignan bloques de 0 bytes\n");
        return;
    }
    if ((p=ObtenerMemoriaShmget(cl,tam,lista_memoria))!=NULL)
        printf ("Asignados %lu bytes en %p\n",(unsigned long) tam, p);
    else
        printf ("Imposible asignar memoria compartida clave %lu:%s\n",(unsigned long) cl,strerror(errno));
}

void shared_aux(char *tr[], char *accion, tListaMemoria *lista_memoria) {
    if (tr[2] == NULL) {
        imprimir_shared(*lista_memoria);
        return;
    }

    if (strcmp(accion, "attach") == 0) {
        ObtenerMemoriaShmget((key_t)strtol(tr[2], NULL, 10), 0, lista_memoria);
    } else if (strcmp(accion, "deattach") == 0) {
        key_t cl = atoi(tr[2]);
        int posicion = firstM(*lista_memoria);
        tElementoMemoria elemento;

        while (posicion <= lastM(*lista_memoria)) {
            elemento = getItemM(posicion, *lista_memoria);
            if (elemento.tipo == SHARED && elemento.informacion.clave == cl) {
                if (shmdt(elemento.direccion) == -1) {
                    perror("ERROR al desadjuntar el bloque de memoria compartida");
                } else {
                    printf("Bloque de memoria con clave %d en la dirección %p fue desadjuntado con éxito.\n",
                            elemento.informacion.clave, elemento.direccion);
                    deleteItemM(posicion, lista_memoria);
                }
                return; // Salir después de encontrar y desadjuntar
            }
            posicion++;
        }

        printf("ERROR: No se encontró un bloque de memoria compartida con clave %d.\n", cl);
    }
}


void Cmd_allocate(char *tr[],tListaMemoria *lista_memoria) {

    if(tr[1]!=NULL) {
        if (strcmp(tr[1], "-malloc") == 0) { //-malloc tam: asigna un bloque malloc de tamano tam
            malloc_aux (tr,"malloc",lista_memoria);
        }else if(strcmp(tr[1],"-mmap") == 0) { //-mmap fich perm: mapea el fichero fich, perm son los permisos
            mmap_aux(tr,"map",lista_memoria);
        }else if(strcmp(tr[1],"-createshared") == 0) { //-createshared cl tam: asigna (creando) el bloque de memoria compartida de clave cl y tamano tam
            do_AllocateCreateshared(tr,lista_memoria);
        }else if(strcmp(tr[1],"-shared") == 0) {//-shared cl: asigna el bloque de memoria compartida (ya existente) de clave cl
            shared_aux(tr,"attach",lista_memoria);
        }
    }
}

bool do_DeallocateDelkey (char *args[]) {
    key_t clave;
    int id;
    char *key=args[0];

    if (key==NULL || (clave=(key_t) strtoul(key,NULL,10))==IPC_PRIVATE){
        printf ("      delkey necesita clave_valida\n");
        return false;
    }
    if ((id=shmget(clave,0,0666))==-1){
        perror ("shmget: imposible obtener memoria compartida");
        return false;
    }
    if (shmctl(id,IPC_RMID,NULL)==-1){
    perror ("shmctl: imposible eliminar memoria compartida\n");
    return false;
    }

    return true;
}

void delkey_aux(char *tr[], tListaMemoria *lista_memoria) {

    // Asegurarse de que tr[2] no sea NULL y convertirlo a clave
    if (tr[2] == NULL) {
        printf("Delkey necesita clave válida\n");
        return;
    }
    key_t clave = (key_t)atoi(tr[2]);

    // Verificar si la clave es válida
    if (clave == IPC_PRIVATE) {
        printf("ERROR: Clave IPC_PRIVATE no es válida para eliminar\n");
        return;
    }

    // Intentar eliminar la clave
    if (!do_DeallocateDelkey(&tr[2])) {
        printf("ERROR: No se pudo eliminar la clave %i\n", clave);
        return;
    }

    // Buscar el segmento de memoria compartida en la lista
    int posicion = firstM(*lista_memoria);
    tElementoMemoria elemento;

    while (posicion <= lastM(*lista_memoria)) {
        elemento = getItemM(posicion, *lista_memoria);

        // Si el tipo de elemento es "SHARED" y la clave coincide
        if (elemento.tipo == SHARED && elemento.informacion.clave == clave) {
            // Desajuntar la memoria
            if (shmdt(elemento.direccion) == -1) {
                perror("ERROR al desadjuntar bloque de memoria compartida");
            } else {
                printf("Bloque de memoria compartida con clave %d en la dirección %p desadjuntado correctamente.\n", clave, elemento.direccion);
            }

            // Eliminar el elemento de la lista
            deleteItemM(posicion, lista_memoria);
            printf("Elemento eliminado de la lista de memoria\n");
            break;  // Ya no necesitamos seguir buscando después de encontrar y eliminar el elemento
        }
        posicion++;
    }
}

void addr_aux(char *tr[], tListaMemoria *lista_memoria) {
    int pos;
    tElementoMemoria elemento;
    void *p;

    // Convertir la dirección de memoria desde el formato de string a void *
    p = (void *) strtol(tr[1], NULL, 16);

    // Recorrer la lista de memoria para buscar el bloque con esa dirección
    for (pos = firstM(*lista_memoria); pos <= lastM(*lista_memoria); pos++) {
        elemento = getItemM(pos, *lista_memoria);

        // Comprobar si la dirección coincide con la proporcionada
        if (elemento.direccion == p) {
            // Si es un bloque tipo MALLOC, liberar la memoria
            if (elemento.tipo == MALLOC) {
                printf("Liberando bloque de memoria tipo MALLOC en la dirección %p\n", elemento.direccion);
                free(elemento.direccion);
            }
            // Si es un bloque tipo SHARED, desvincularlo (detach)
            else if (elemento.tipo == SHARED) {
                printf("Desasociando bloque de memoria compartida con clave %d en la dirección %p\n", elemento.informacion.clave, elemento.direccion);
                if (shmdt(elemento.direccion) == -1) {
                    perror("ERROR: No se pudo desasociar la memoria compartida.");
                } else {
                    printf("Bloque de memoria compartida desasociado correctamente.\n");
                }
            }

            // Eliminar el elemento de la lista de memoria después de liberarlo o desvincularlo
            deleteItemM(pos, lista_memoria);
            printf("Elemento con dirección %p eliminado de la lista de memoria.\n", elemento.direccion);
            return;  // Terminamos la búsqueda una vez encontramos y liberamos o desvinculamos el bloque
        }
    }

    // Si no se encontró la dirección en la lista de memoria
    printf("No se encontró un bloque de memoria con la dirección %p.\n", p);
}

void Cmd_deallocate(char *tr[], tListaMemoria *lista_memoria) {
    if (tr[1] != NULL) {
        if (strcmp(tr[1], "-malloc") == 0) {
            malloc_aux(tr, "free", lista_memoria);
        } else if (strcmp(tr[1], "-mmap") == 0) {
            mmap_aux(tr, "unmap", lista_memoria);
        } else if (strcmp(tr[1], "-shared") == 0) {
            shared_aux(tr, "deattach", lista_memoria);
        } else if (strcmp(tr[1], "-delkey") == 0) {
            delkey_aux(tr, lista_memoria);
        } if (strncmp(tr[1], "0x", 2) == 0) {
            addr_aux(tr,lista_memoria);
        }
    }
}

void LlenarMemoria(void *p, size_t cont, unsigned char byte) {
    unsigned char *arr = (unsigned char *)p;
    size_t i;

    for (i = 0; i < cont; i++) {
        arr[i] = byte;
    }
}

void Cmd_memfill(char *tr[]) {
    size_t tamaño = 128; //Tamaño por defecto
    unsigned char caracter = 'A';  // Valor por defecto: 'A' (41)
    void *p;


   if(tr[1] != NULL ) {
       // Obtener la dirección
       uintptr_t direccion = (uintptr_t)strtoull(tr[1], NULL, 0); //Hacemos al conversion de un string a una direccion de memoria
       p = (void *)direccion;

       if(tr[2] != NULL) { //Si le pasamos un tamnaño (si no se lo pasamos usa el tanmaño por defecto)
           // Obtener el tamaño desde el segundo argumento
           tamaño = (size_t)strtoull(tr[2], NULL, 10);
       }

       if(tr[3] != NULL) { //Si le pasamos un caracter usamos ese si no el por defecto
           caracter = (unsigned char)tr[3][0];
       }
         // Llenar la memoria
        LlenarMemoria(p, tamaño, caracter);

        printf("Llenando %zu bytes de memoria con el byte '%c' (%x) a partir de la dirección %p.\n",
           tamaño, caracter, caracter, p);

   } else {
       printf("Para usar el comando memfill introduce una dirección \n");
   }
}

void Cmd_memdump(char *tr[]) {
    size_t tamaño = 25;  // Tamaño por defecto

    if (tr[1] != NULL) {  // Si se proporciona la dirección de memoria
        // Convertir el primer argumento+ a un puntero de memoria
        uintptr_t direccion = (uintptr_t)strtoull(tr[1], NULL, 0);
        void *p = (void *)direccion;

        // Si se proporciona un tamaño, actualizar el tamaño
        if (tr[2] != NULL) {
            tamaño = (size_t)strtoull(tr[2], NULL, 10);
        }

        printf("Volcando %zu bytes desde la direccion %s:\n", tamaño, tr[1]);

        // Convertir la dirección a un puntero a unsigned char para iterar byte por byte
        unsigned char *byte_ptr = (unsigned char *)p;

        // Iterar sobre los bytes de la memoria
        for (size_t i = 0; i < tamaño; i++) {

            if (byte_ptr[i] >= 32 && byte_ptr[i] <= 126) {
                printf("%c", byte_ptr[i]);
            } else {
                printf(". ");  // Si no es imprimible, imprimir un punto
            }

        }

        printf("\n");

        // Iterar sobre los bytes de la memoria
        for (size_t i = 0; i < tamaño; i++) {
            // Imprimir el valor en formato hexadecimal
            printf("(%02x) ", byte_ptr[i]);

            // Salto de línea cada 25 bytes
            if ((i + 1) % 25 == 0) {
                printf("\n");
            }
        }

        // Si el tamaño no es múltiplo de 25, asegurar que termine con un salto de línea
        if (tamaño % 25 != 0) {
            printf("\n");
        }
    }
}
void funcs_aux() {
    // Punteros a funciones del programa
    void (*ptr_func1)() = Cmd_memfill;
    void (*ptr_func2)() = Cmd_memdump;
    void (*ptr_func3)() = Cmd_allocate;

    // Punteros a funciones de la biblioteca estándar
    int (*ptr_printf)(const char *, ...) = printf;
    void *(*ptr_malloc)(size_t) = malloc;
    void (*ptr_free)(void*) = free;

    // Imprimir direcciones de las funciones del programa
    printf("FUNCIONES PROGRAMA: %p  %p  %p\n", (void *)ptr_func1, (void *)ptr_func2, (void *)ptr_func3);

    // Imprimir direcciones de las funciones de la biblioteca
    printf("FUNCIONES BIBLIOTECA: %p  %p  %p\n", (void *)ptr_printf, (void *)ptr_malloc, (void *)ptr_free);
}

int gn1, gn2, gn3, g1 = 0, g2 = 2, g3 = 3; //VARIABLES GLOBALES

void vars_aux() {
    float l1 = 3.14, l2 = 1.618, l3 = 2.718;
    static double d1 = 3.14, d2 = 1.618, d3 = 2.718, dn1, dn2, dn3;

    printf("VARIABLES LOCALES: %p  %p  %p\n",&l1,&l2,&l3);
    printf("VARIABLES GLOBALES (EXTERNAL): %p  %p  %p\n",&g1,&g2,&g3);
    printf("(N.I) VARIABLES GLOBALES (EXTERNAL): %p  %p  %p\n",&gn1,&gn2,&gn3);
    printf("VARIABLES ESTATICAS : %p  %p  %p\n",&d1,&d2,&d3);
    printf("(N.I) VARIABLES ESTATICAS : %p  %p  %p\n",&dn1,&dn2,&dn3);
}

void all_aux(tListaMemoria *lista_memoria) {
    funcs_aux();
    vars_aux();
    imprimir_malloc(*lista_memoria);
}


void Do_pmap (void) /*sin argumentos*/
 { pid_t pid;       /*hace el pmap (o equivalente) del proceso actual*/
    char elpid[32];
    char *argv[4]={"pmap",elpid,NULL};

    sprintf (elpid,"%d", (int) getpid());
    if ((pid=fork())==-1){
        perror ("Imposible crear proceso");
        return;
    }
    if (pid==0){
        if (execvp(argv[0],argv)==-1)
            perror("cannot execute pmap (linux, solaris)");

        argv[0]="procstat"; argv[1]="vm"; argv[2]=elpid; argv[3]=NULL;
        if (execvp(argv[0],argv)==-1)/*No hay pmap, probamos procstat FreeBSD */
            perror("cannot execute procstat (FreeBSD)");

        argv[0]="procmap",argv[1]=elpid;argv[2]=NULL;
        if (execvp(argv[0],argv)==-1)  /*probamos procmap OpenBSD*/
            perror("cannot execute procmap (OpenBSD)");

        argv[0]="vmmap"; argv[1]="-interleave"; argv[2]=elpid;argv[3]=NULL;
        if (execvp(argv[0],argv)==-1) /*probamos vmmap Mac-OS*/
            perror("cannot execute vmmap (Mac-OS)");
        exit(1);
    }
    waitpid (pid,NULL,0);
 }


void Cmd_memory(char *tr[],tListaMemoria *lista_memoria) {

    if (tr[1] != NULL) {
        if (strcmp(tr[1], "-funcs") == 0) {
            funcs_aux();
        }else if (strcmp(tr[1], "-vars") == 0) {
            vars_aux();
        }else if (strcmp(tr[1], "-blocks") == 0) {
            imprimir_malloc(*lista_memoria);
        }else if (strcmp(tr[1], "-all") == 0) {
            all_aux(lista_memoria);
        }else if (strcmp(tr[1], "-pmap") == 0) {
            Do_pmap();
        }
    }else {
       all_aux(lista_memoria);
    }
}


ssize_t LeerFichero (char *f, void *p, size_t cont)
{
    struct stat s;
    ssize_t  n;
    int df,aux;

    if (stat (f,&s)==-1 || (df=open(f,O_RDONLY))==-1)
        return -1;
    if (cont==-1)   /* si pasamos -1 como bytes a leer lo leemos entero*/
        cont=s.st_size;
    if ((n=read(df,p,cont))==-1){
        aux=errno;
        close(df);
        errno=aux;
        return -1;
    }

    close (df);
    return n;
}

void Cmd_ReadFile (char *tr[]) {
    size_t cont=-1;  /*si no pasamos tamano se lee entero */
    ssize_t n;
    if (tr[1]==NULL || tr[2]==NULL){
        printf ("faltan parametros\n");
        return;
    }

    // Convertir el primer argumento a un puntero de memoria
    uintptr_t direccion = (uintptr_t)strtoull(tr[2], NULL, 0);
    void *p = (void *)direccion;

    if (tr[3]!=NULL)
        cont=(size_t) atoll(tr[3]);

    if ((n=LeerFichero(tr[1],p,cont))==-1)
        perror ("Imposible leer fichero");
    else{
        printf("Leídos %lld bytes de %s en %p\n", (long long)n, tr[1], p);
    }
}
void Cmd_WriteFile (char *tr[]) {
    FILE *fp = NULL;
    size_t cont;


    if(strcmp(tr[1], "-o") == 0) { //Si tenemos puesta la opcion -o

        if(tr[2]==NULL || tr[3]==NULL || tr[4]==NULL) {
            printf ("faltan parametros: writefile [-o] file addr cont\n");
            return;
        }

        // Obtener la dirección
        uintptr_t direccion = (uintptr_t)strtoull(tr[3], NULL, 0); //Hacemos al conversion de un string a una direccion de memoria
        void *addr = (void *)direccion;

        fp = fopen (tr[2], "wb"); //Abrimos el fichero para sobrescribirlo

        if(!fp) {
            perror ("Imposible abrir el archivo");
            return;
        }

            cont = atoi(tr[4]);


        //Escribir los datos desde la direccion
        if(fwrite(addr,1,cont,fp) != cont) {
            perror("Error al escribitr el archivo");
        }else {
            printf("Escritura completada: %zu bytes en %s\n", cont, tr[2]);
        }
        fclose(fp);

    }else {

        if(tr[1]==NULL || tr[2]==NULL || tr[3]!=NULL) {
            printf ("faltan parametros: writefile [-o] file addr cont\n");
            return;
        }

        // Obtener la dirección
        uintptr_t direccion = (uintptr_t)strtoull(tr[2], NULL, 0); //Hacemos al conversion de un string a una direccion de memoria
        void *addr = (void *)direccion;

        fp = fopen (tr[1], "wbx");

        if(!fp) {
            perror ("Imposible abrir el archivo");
            return;
        }

            cont = atoi(tr[3]);


        //Escribir los datos desde la direccion
        if(fwrite(addr,1,cont,fp) != cont) {
            perror("Error al escribitr el archivo");
        }else {
            printf("Escritura completada: %zu bytes en %s\n", cont, tr[1]);
        }
        fclose(fp);
    }
}

// Leer desde un descriptor de archivo hacia una dirección de memoria
ssize_t LeerDescriptor(int fd, void *p, size_t cont) {
    struct stat s;
    ssize_t n;
    int aux;

    // Obtenemos la información del descriptor para verificar el tamaño si es necesario
    if (fstat(fd, &s) == -1) {
        return -1;
    }

    // Si se pasa -1 como tamaño, leemos el archivo entero
    if (cont == (size_t)-1) {
        cont = s.st_size;
    }

    // Leemos los datos del descriptor en el búfer proporcionado
    if ((n = read(fd, p, cont)) == -1) {
        aux = errno;  // Guardamos el error antes de cualquier otra operación
        errno = aux;
        return -1;
    }

    return n; // Devuelve el número de bytes leídos
}
void Cmd_Read(char *tr[]) {
    size_t cont = (size_t)-1; // Por defecto, leer todo
    ssize_t n;

    if (tr[1] == NULL || tr[2] == NULL) {
        printf("Faltan parámetros\n");
        return;
    }

    // Convertir el segundo argumento a una dirección de memoria
    uintptr_t direccion = (uintptr_t)strtoull(tr[2], NULL, 0);
    void *p = (void *)direccion;

    if (tr[3] != NULL) {
        cont = (size_t)atoll(tr[3]);
    }

    // Convertir el primer argumento en un descriptor de archivo (lo asumimos como entero)
    int fd = atoi(tr[1]);

    // Llamamos a la función para leer desde el descriptor
    if ((n = LeerDescriptor(fd, p, cont)) == -1) {
        perror("Imposible leer desde el descriptor");
    } else {
        printf("Leídos %lld bytes del descriptor %d en %p\n", (long long)n, fd, p);
    }
}


void Cmd_Write(char *tr[]) {
    size_t cont;
    ssize_t written;

    // Verificamos que los parámetros son suficientes
    if (tr[1] == NULL || tr[2] == NULL || tr[3] == NULL) {
        printf("Faltan parámetros: writefile fd addr cont\n");
        return;
    }

    // Convertir descriptor de archivo (fd) a entero
    int fd = atoi(tr[1]);

    // Convertir dirección de memoria
    uintptr_t direccion = (uintptr_t)strtoull(tr[2], NULL, 0);
    void *addr = (void *)direccion;

    // Leer la cantidad de bytes a escribir
    cont = (size_t)atoi(tr[3]);

    // Escribir los datos desde la dirección al descriptor
    written = write(fd, addr, cont);
    if (written == -1) {
        perror("Error al escribir en el descriptor");
    } else if ((size_t)written != cont) {
        fprintf(stderr, "Error: no se escribieron todos los bytes (%zd de %zu)\n", written, cont);
    } else {
        printf("Escritura completada: %zd bytes en descriptor %d\n", written, fd);
    }
}


