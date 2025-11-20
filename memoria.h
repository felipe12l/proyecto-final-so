#ifndef MEMORIA_H
#define MEMORIA_H

#include "proceso.h"

#define TAM_PAGINA 4096       // 4KB por página
#define NUM_MARCOS_DEFAULT 64 // 64 marcos de página (256KB total)

// Algoritmos de reemplazo de páginas
typedef enum
{
    FIFO,
    LRU
} AlgoritmoReemplazo;

// Estructura de un marco de página
typedef struct
{
    int ocupado;        // 0 = libre, 1 = ocupado
    int pid;            // ID del proceso dueño
    int num_pagina;     // Número de página lógica
    int tiempo_carga;   // Timestamp de cuándo se cargó
    int ultimo_acceso;  // Timestamp del último acceso (para LRU)
    int bit_referencia; // Bit de referencia
    int bit_modificado; // Bit de modificación (dirty bit)
} MarcoPagina;

// Tabla de páginas de un proceso
typedef struct
{
    int pid;           // Proceso dueño
    int num_paginas;   // Páginas totales del proceso
    int *marco_fisico; // Array: índice=página lógica, valor=marco físico (-1 si no está)
    int *valido;       // Bit de validez para cada página
} TablaPaginas;

// Gestor de memoria del sistema
typedef struct
{
    MarcoPagina *marcos;
    int num_marcos;
    TablaPaginas *tablas_paginas; // Array de tablas, una por proceso
    int num_procesos;
    int capacidad_procesos;
    int tiempo_actual;

    // Estadísticas
    int fallos_pagina;
    int num_reemplazos;
    int num_accesos;

    // Algoritmo de reemplazo
    AlgoritmoReemplazo algoritmo;

    // Cola FIFO para reemplazo
    int *cola_fifo;
    int frente_fifo;
    int final_fifo;
} GestorMemoria;

// Funciones de gestión de memoria
GestorMemoria *crear_gestor_memoria(int num_marcos, AlgoritmoReemplazo algoritmo);
void destruir_gestor_memoria(GestorMemoria *gm);
void registrar_proceso_memoria(GestorMemoria *gm, Proceso *p);
int acceder_pagina(GestorMemoria *gm, int pid, int num_pagina, int tiempo);
int reemplazar_pagina(GestorMemoria *gm, int pid, int num_pagina, int tiempo);
void liberar_memoria_proceso(GestorMemoria *gm, int pid);
void imprimir_estado_memoria(GestorMemoria *gm);
void imprimir_estadisticas_memoria(GestorMemoria *gm);
int encontrar_marco_fifo(GestorMemoria *gm);
int encontrar_marco_lru(GestorMemoria *gm);

#endif
