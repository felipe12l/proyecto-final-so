# Compilador y flags
CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -O2
TARGET = simulador

# Archivos fuente y objeto
SOURCES = main.c proceso.c memoria.c archivo.c planificador.c
OBJECTS = $(SOURCES:.c=.o)
HEADERS = proceso.h memoria.h archivo.h planificador.h

# Regla por defecto
all: $(TARGET)

# Compilar el ejecutable
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)
	@echo "Compilacion exitosa!"
	@echo "Ejecute con: ./$(TARGET)"

# Compilar archivos objeto
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Limpiar archivos compilados
clean:
	rm -f $(OBJECTS) $(TARGET)
	@echo "Limpieza completada"

# Limpiar y recompilar
rebuild: clean all

# Ejecutar el simulador con configuración por defecto
run: $(TARGET)
	./$(TARGET)

# Ejecutar simulación rápida
quick: $(TARGET)
	./$(TARGET) -a rr -q 4 -r lru -m 32 -p 10 -f 5

# Ejecutar comparación de algoritmos
compare: $(TARGET)
	@echo "Round Robin:"
	./$(TARGET) -a rr -q 4 -r lru -m 32 -p 10 -f 5
	@echo "\nShortest Job First:"
	./$(TARGET) -a sjf -r lru -m 32 -p 10 -f 5
	@echo "\nPrioridad:"
	./$(TARGET) -a prioridad -r lru -m 32 -p 10 -f 5

# Prueba de FIFO vs LRU
test-memory: $(TARGET)
	@echo "Probando FIFO:"
	./$(TARGET) -a rr -q 4 -r fifo -m 16 -p 8 -f 3
	@echo "\nProbando LRU:"
	./$(TARGET) -a rr -q 4 -r lru -m 16 -p 8 -f 3

# Ayuda
help:
	@echo "Makefile del Simulador de Sistema Operativo"
	@echo ""
	@echo "Uso:"
	@echo "  make          - Compilar el proyecto"
	@echo "  make clean    - Eliminar archivos compilados"
	@echo "  make rebuild  - Limpiar y recompilar"
	@echo "  make run      - Compilar y ejecutar modo interactivo"
	@echo "  make quick    - Ejecutar simulacion rapida"
	@echo "  make compare  - Comparar algoritmos de planificacion"
	@echo "  make test-memory - Comparar FIFO vs LRU"
	@echo "  make help     - Mostrar esta ayuda"
	@echo ""
	@echo "Opciones de linea de comandos:"
	@echo "  -a <algoritmo>  : rr (Round Robin), sjf, prioridad"
	@echo "  -q <quantum>    : Quantum para Round Robin"
	@echo "  -r <reemplazo>  : fifo, lru"
	@echo "  -m <marcos>     : Numero de marcos de memoria"
	@echo "  -p <procesos>   : Numero de procesos"
	@echo "  -f <archivos>   : Numero de archivos"
	@echo ""
	@echo "Ejemplo:"
	@echo "  ./simulador -a sjf -r lru -m 64 -p 15 -f 8"

.PHONY: all clean rebuild run quick compare test-memory help
