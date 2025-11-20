#ifndef PLANIFICADOR_H
#define PLANIFICADOR_H

#include "proceso.h"
#include "memoria.h"
#include "archivo.h"

// Algoritmos de planificación
typedef enum
{
    ROUND_ROBIN,
    SJF, // Shortest Job First
    PRIORIDAD
} AlgoritmoScheduling;

// Cola de procesos
typedef struct
{
    Proceso *frente;
    Proceso *final;
    int cantidad;
} ColaProcesos;

// Planificador del sistema
typedef struct
{
    AlgoritmoScheduling algoritmo;
    ColaProcesos cola_listos;
    ColaProcesos cola_bloqueados;
    Proceso *proceso_actual;
    int quantum; // Para Round Robin (unidades de tiempo)
    int tiempo_actual;

    // Referencias a otros componentes
    GestorMemoria *gestor_memoria;
    GestorArchivos *gestor_archivos;

    // Métricas
    int procesos_completados;
    int procesos_creados;
    double tiempo_espera_total;
    double tiempo_retorno_total;
    double tiempo_respuesta_total;
    int tiempo_cpu_usado;
    int cambios_contexto;
    int tiempo_inicio;
} Planificador;

// Funciones de cola
void inicializar_cola(ColaProcesos *cola);
void encolar(ColaProcesos *cola, Proceso *p);
Proceso *desencolar(ColaProcesos *cola);
Proceso *desencolar_sjf(ColaProcesos *cola);
Proceso *desencolar_prioridad(ColaProcesos *cola);
int cola_vacia(ColaProcesos *cola);
void destruir_cola(ColaProcesos *cola);
void imprimir_cola(ColaProcesos *cola, const char *nombre);

// Funciones del planificador
Planificador *crear_planificador(AlgoritmoScheduling algoritmo, int quantum,
                                 GestorMemoria *gm, GestorArchivos *ga);
void destruir_planificador(Planificador *sched);
void agregar_proceso(Planificador *sched, Proceso *p);
void ejecutar_simulacion(Planificador *sched, int duracion_maxima);
void ejecutar_ciclo(Planificador *sched);
Proceso *seleccionar_siguiente_proceso(Planificador *sched);
void cambio_contexto(Planificador *sched, Proceso *nuevo_proceso);
void simular_ejecucion_proceso(Planificador *sched, int tiempo_ejecucion);
void actualizar_procesos_bloqueados(Planificador *sched);
void calcular_metricas(Planificador *sched);
void imprimir_metricas(Planificador *sched);
void imprimir_estado_sistema(Planificador *sched);

#endif
