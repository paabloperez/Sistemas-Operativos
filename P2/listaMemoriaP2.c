#include "listaMemoriaP2.h"


// Crear una lista vacía
void createEmptyListM(tListaMemoria *L) {
    L->posicionUltimo = -1;  // Inicializa la posición del último elemento como -1 (indicando que la lista está vacía)
}

// Verificar si la lista está vacía
bool isEmptyListM(tListaMemoria L) {
    return (L.posicionUltimo == -1);  // Si la posición del último elemento es -1, la lista está vacía
}

// Obtener el primer elemento de la lista
int firstM(tListaMemoria L) {
    if (isEmptyListM(L)) {
        return -1;  // Retorna -1 si la lista está vacía
    }
    return 0;  // Retorna la posición del primer elemento (si la lista no está vacía)
}

// Obtener el último elemento de la lista
int lastM(tListaMemoria L) {
    return L.posicionUltimo;  // Retorna la posición del último elemento
}

// Obtener el elemento en una posición específica
tElementoMemoria getItemM(int pos, tListaMemoria L) {
    return L.elementos[pos];
}

// Eliminar un elemento de la lista en una posición específica
void deleteItemM(int pos, tListaMemoria *L) {
    if (pos >= 0 && pos <= L->posicionUltimo) {
        for (int i = pos; i < L->posicionUltimo; i++) {
            L->elementos[i] = L->elementos[i + 1];  // Desplaza los elementos hacia la izquierda
        }
        L->posicionUltimo--;  // Actualiza la posición del último elemento
    }
}

// Eliminar toda la lista
void deleteListM(tListaMemoria *L) {
    L->posicionUltimo = -1;  // Resetea la posición del último elemento, dejando la lista vacía
}

// Insertar un nuevo item en la lista
bool insertItemM(tElementoMemoria item, tListaMemoria *L) {
    if (L->posicionUltimo < MAXDATA - 1) {  // Verifica que haya espacio para agregar más elementos
        L->posicionUltimo++;  // Incrementa la posición del último elemento
        L->elementos[L->posicionUltimo] = item;  // Inserta el nuevo item
        return true;
    }
    return false;  // Retorna false si la lista está llena
}

void imprimir_malloc(tListaMemoria L) {
  int posicion;
  tElementoMemoria elemento;

  printf("******Lista de bloques asignados malloc para el proceso %d\n", getpid());

  if(isEmptyListM(L)){
    printf("Aún no se han asignado bloques\n");
  }else{
    for (posicion = firstM(L) ; posicion <= lastM(L); posicion++) {
      elemento = getItemM(posicion, L);

      if(elemento.tipo == MALLOC) {

        printf("%p\t%lu bytes\t %s malloc\n",elemento.direccion,elemento.tamaño,elemento.fecha);

        }

    }
  }
}

void imprimir_mmap(tListaMemoria L) {
  int posicion ;
  tElementoMemoria elemento;

    printf("******Lista de bloques asignados mmap para el proceso %d\n", getpid());

    if(isEmptyListM(L)){
        printf("Aún no se ha mapeado ningun archivo\n");
    }else{

        for (posicion = firstM(L); posicion <= lastM(L); posicion++) {
            elemento = getItemM(posicion, L);

            if(elemento.tipo == MMAP) {

                printf("%p\t %s %s (descriptor %i)\n",elemento.direccion,elemento.fecha,elemento.informacion.archivo.nombre,elemento.informacion.archivo.descriptor);

            }
        }
    }
}

void imprimir_shared(tListaMemoria L) {
    int posicion ;
    tElementoMemoria elemento;

    printf("******Lista de bloques asignados shared para el proceso %d\n",getpid());

    if(isEmptyListM(L)){
        printf("Aún no se ha mapeado ningun archivo\n");
    }else{

        for (posicion = firstM(L); posicion <= lastM(L); posicion++) {
            elemento = getItemM(posicion, L);

            if(elemento.tipo == SHARED) {

                printf("%p\t %s %d (KEY %i)\n",elemento.direccion,elemento.fecha,elemento.tipo,elemento.informacion.clave);

            }
        }
    }
}





