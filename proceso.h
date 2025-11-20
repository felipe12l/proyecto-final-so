#ifndef PROCESO_H
#define PROCESO_H

// Estados posibles de un proceso
typedef enum
{
    NUEVO,
    LISTO,
    EJECUTANDO,
    BLOQUEADO,
    TERMINADO
} EstadoProceso;

// Estructura de Control de Proceso (PCB)
typedef struct Proceso
{
    int pid;              // Identificador único del proceso
    char nombre[32];      // Nombre descriptivo
    EstadoProceso estado; // Estado actual
    int prioridad;        // Prioridad (0-10, menor número = mayor prioridad)
    int tiempo_llegada;   // Tiempo de llegada a la cola
    int tiempo_rafaga;    // Tiempo total de CPU necesario
    int tiempo_restante;  // Tiempo que falta por ejecutar
    int tiempo_espera;    // Tiempo total esperando en cola
    int tiempo_retorno;   // Tiempo desde llegada hasta finalización
    int tiempo_respuesta; // Tiempo hasta primera ejecución (-1 si no ha ejecutado)
    int quantum_restante; // Quantum restante (para Round Robin)

    // Gestión de memoria
    int tamanio_memoria; // Tamaño en páginas
    int *paginas;        // Array de páginas que puede referenciar
    int num_paginas;     // Número de páginas del proceso
    int pagina_actual;   // Página que está accediendo actualmente

    // Gestión de archivos
    char **archivos_abiertos; // Lista de nombres de archivos que usa
    int num_archivos;         // Cantidad de archivos
    int archivo_actual;       // Índice del archivo que está accediendo (-1 si ninguno)
    int modo_acceso;          // 0 = lectura, 1 = escritura

    struct Proceso *siguiente; // Para lista enlazada
} Proceso;

// Funciones para manejo de procesos
Proceso *crear_proceso(int pid, const char *nombre, int prioridad, int tiempo_rafaga,
                       int num_paginas, int num_archivos);
void destruir_proceso(Proceso *p);
void imprimir_proceso(Proceso *p);
const char *estado_a_string(EstadoProceso estado);

// Generación de procesos aleatorios
Proceso *generar_proceso_aleatorio(int pid);
void asignar_archivos_proceso(Proceso *p, int num_archivos_sistema);

#endif
