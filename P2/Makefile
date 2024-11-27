# Variables
CC = gcc
CFLAGS = -Wall -g  # Activar advertencias y depuración
TARGET = P2

# Archivos fuente
SRC = main.c comandosP0.c lista.c comandosP1.c comandosP2.c listaMemoriaP2.c

# Regla por defecto para crear el ejecutable
all: $(TARGET)

# Regla para crear el ejecutable P2
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# Regla para limpiar los archivos generados
clean:
	rm -f $(TARGET)
