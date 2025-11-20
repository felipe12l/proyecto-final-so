#include "planificador.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ========== Funciones de Cola ==========

void inicializar_cola(ColaProcesos *cola)
{
    cola->frente = NULL;
    cola->final = NULL;
    cola->cantidad = 0;
}

void encolar(ColaProcesos *cola, Proceso *p)
{
    if (!cola || !p)
        return;

    p->siguiente = NULL;

    if (cola->final == NULL)
    {
        cola->frente = p;
        cola->final = p;
    }
    else
    {
        cola->final->siguiente = p;
        cola->final = p;
    }

    cola->cantidad++;
}

Proceso *desencolar(ColaProcesos *cola)
{
    if (!cola || cola->frente == NULL)
        return NULL;

    Proceso *p = cola->frente;
    cola->frente = cola->frente->siguiente;

    if (cola->frente == NULL)
    {
        cola->final = NULL;
    }

    p->siguiente = NULL;
    cola->cantidad--;

    return p;
}

Proceso *desencolar_sjf(ColaProcesos *cola)
{
    if (!cola || cola->frente == NULL)
        return NULL;

    // Buscar el proceso con menor tiempo restante
    Proceso *anterior = NULL;
    Proceso *actual = cola->frente;
    Proceso *min_anterior = NULL;
    Proceso *min_proceso = cola->frente;
    int min_tiempo = cola->frente->tiempo_restante;

    while (actual != NULL)
    {
        if (actual->tiempo_restante < min_tiempo)
        {
            min_tiempo = actual->tiempo_restante;
            min_proceso = actual;
            min_anterior = anterior;
        }
        anterior = actual;
        actual = actual->siguiente;
    }

    // Remover de la lista
    if (min_anterior == NULL)
    {
        // Es el primero
        cola->frente = min_proceso->siguiente;
        if (cola->frente == NULL)
        {
            cola->final = NULL;
        }
    }
    else
    {
        min_anterior->siguiente = min_proceso->siguiente;
        if (min_proceso == cola->final)
        {
            cola->final = min_anterior;
        }
    }

    min_proceso->siguiente = NULL;
    cola->cantidad--;

    return min_proceso;
}

Proceso *desencolar_prioridad(ColaProcesos *cola)
{
    if (!cola || cola->frente == NULL)
        return NULL;

    // Buscar el proceso con mayor prioridad (menor número)
    Proceso *anterior = NULL;
    Proceso *actual = cola->frente;
    Proceso *max_anterior = NULL;
    Proceso *max_proceso = cola->frente;
    int max_prioridad = cola->frente->prioridad;

    while (actual != NULL)
    {
        if (actual->prioridad < max_prioridad)
        {
            max_prioridad = actual->prioridad;
            max_proceso = actual;
            max_anterior = anterior;
        }
        anterior = actual;
        actual = actual->siguiente;
    }

    // Remover de la lista
    if (max_anterior == NULL)
    {
        cola->frente = max_proceso->siguiente;
        if (cola->frente == NULL)
        {
            cola->final = NULL;
        }
    }
    else
    {
        max_anterior->siguiente = max_proceso->siguiente;
        if (max_proceso == cola->final)
        {
            cola->final = max_anterior;
        }
    }

    max_proceso->siguiente = NULL;
    cola->cantidad--;

    return max_proceso;
}

int cola_vacia(ColaProcesos *cola)
{
    return (cola == NULL || cola->frente == NULL);
}

void destruir_cola(ColaProcesos *cola)
{
    if (!cola)
        return;

    while (!cola_vacia(cola))
    {
        Proceso *p = desencolar(cola);
        destruir_proceso(p);
    }
}

void imprimir_cola(ColaProcesos *cola, const char *nombre)
{
    if (!cola)
        return;

    printf("  Cola %s (%d): [", nombre, cola->cantidad);
    Proceso *p = cola->frente;
    while (p != NULL)
    {
        printf("P%d", p->pid);
        if (p->siguiente != NULL)
            printf(", ");
        p = p->siguiente;
    }
    printf("]\n");
}

// ========== Funciones del Planificador ==========

Planificador *crear_planificador(AlgoritmoScheduling algoritmo, int quantum,
                                 GestorMemoria *gm, GestorArchivos *ga)
{
    Planificador *sched = (Planificador *)malloc(sizeof(Planificador));
    if (!sched)
    {
        fprintf(stderr, "Error: No se pudo asignar memoria para el planificador\n");
        return NULL;
    }

    sched->algoritmo = algoritmo;
    sched->quantum = quantum;
    inicializar_cola(&sched->cola_listos);
    inicializar_cola(&sched->cola_bloqueados);
    sched->proceso_actual = NULL;
    sched->tiempo_actual = 0;
    sched->gestor_memoria = gm;
    sched->gestor_archivos = ga;

    // Inicializar métricas
    sched->procesos_completados = 0;
    sched->procesos_creados = 0;
    sched->tiempo_espera_total = 0;
    sched->tiempo_retorno_total = 0;
    sched->tiempo_respuesta_total = 0;
    sched->tiempo_cpu_usado = 0;
    sched->cambios_contexto = 0;
    sched->tiempo_inicio = 0;

    return sched;
}

void destruir_planificador(Planificador *sched)
{
    if (!sched)
        return;

    destruir_cola(&sched->cola_listos);
    destruir_cola(&sched->cola_bloqueados);

    if (sched->proceso_actual)
    {
        destruir_proceso(sched->proceso_actual);
    }

    free(sched);
}

void agregar_proceso(Planificador *sched, Proceso *p)
{
    if (!sched || !p)
        return;

    p->estado = LISTO;
    p->tiempo_llegada = sched->tiempo_actual;
    encolar(&sched->cola_listos, p);
    sched->procesos_creados++;

    // Registrar en gestor de memoria
    if (sched->gestor_memoria)
    {
        registrar_proceso_memoria(sched->gestor_memoria, p);
    }
}

Proceso *seleccionar_siguiente_proceso(Planificador *sched)
{
    if (!sched || cola_vacia(&sched->cola_listos))
    {
        return NULL;
    }

    Proceso *p = NULL;

    switch (sched->algoritmo)
    {
    case ROUND_ROBIN:
        p = desencolar(&sched->cola_listos);
        if (p)
            p->quantum_restante = sched->quantum;
        break;

    case SJF:
        p = desencolar_sjf(&sched->cola_listos);
        break;

    case PRIORIDAD:
        p = desencolar_prioridad(&sched->cola_listos);
        break;
    }

    return p;
}

void cambio_contexto(Planificador *sched, Proceso *nuevo_proceso)
{
    if (!sched)
        return;

    if (sched->proceso_actual != NULL)
    {
        sched->cambios_contexto++;
    }

    sched->proceso_actual = nuevo_proceso;

    if (nuevo_proceso != NULL)
    {
        nuevo_proceso->estado = EJECUTANDO;

        // Registrar tiempo de respuesta si es la primera vez que ejecuta
        if (nuevo_proceso->tiempo_respuesta == -1)
        {
            nuevo_proceso->tiempo_respuesta = sched->tiempo_actual - nuevo_proceso->tiempo_llegada;
        }
    }
}

void simular_ejecucion_proceso(Planificador *sched, int tiempo_ejecucion)
{
    if (!sched || !sched->proceso_actual)
        return;

    Proceso *p = sched->proceso_actual;

    for (int t = 0; t < tiempo_ejecucion && p->tiempo_restante > 0; t++)
    {
        // Simular acceso a memoria (cada 2 unidades de tiempo)
        if (t % 2 == 0 && sched->gestor_memoria)
        {
            int pagina = p->paginas[rand() % p->num_paginas];
            acceder_pagina(sched->gestor_memoria, p->pid, pagina, sched->tiempo_actual);
        }

        // Simular acceso a archivo (probabilidad 20%)
        if (p->num_archivos > 0 && (rand() % 100) < 20 && sched->gestor_archivos)
        {
            if (p->archivo_actual == -1)
            {
                // Intentar abrir un archivo
                int idx_archivo = rand() % p->num_archivos;
                char *nombre_archivo = p->archivos_abiertos[idx_archivo];
                ModoAcceso modo = (rand() % 2 == 0) ? LECTURA : ESCRITURA;

                int resultado = solicitar_acceso_archivo(sched->gestor_archivos,
                                                         nombre_archivo, p->pid, modo);

                if (resultado == 0)
                {
                    // Bloqueado por conflicto
                    p->estado = BLOQUEADO;
                    p->archivo_actual = idx_archivo;
                    p->modo_acceso = modo;
                    encolar(&sched->cola_bloqueados, p);
                    sched->proceso_actual = NULL;
                    return;
                }
                else if (resultado == 1)
                {
                    p->archivo_actual = idx_archivo;
                    p->modo_acceso = modo;
                }
            }
            else
            {
                // Liberar archivo actual (20% de probabilidad)
                if ((rand() % 100) < 20)
                {
                    char *nombre_archivo = p->archivos_abiertos[p->archivo_actual];
                    liberar_acceso_archivo(sched->gestor_archivos, nombre_archivo, p->pid);
                    p->archivo_actual = -1;
                }
            }
        }

        p->tiempo_restante--;
        sched->tiempo_actual++;
        sched->tiempo_cpu_usado++;

        // Decrementar quantum si es Round Robin
        if (sched->algoritmo == ROUND_ROBIN)
        {
            p->quantum_restante--;
            if (p->quantum_restante <= 0 && p->tiempo_restante > 0)
            {
                // Quantum agotado, devolver a cola
                p->estado = LISTO;
                encolar(&sched->cola_listos, p);
                sched->proceso_actual = NULL;
                return;
            }
        }
    }

    // Verificar si el proceso terminó
    if (p->tiempo_restante <= 0)
    {
        p->estado = TERMINADO;
        p->tiempo_retorno = sched->tiempo_actual - p->tiempo_llegada;

        // Liberar archivos abiertos
        if (p->archivo_actual != -1 && sched->gestor_archivos)
        {
            char *nombre_archivo = p->archivos_abiertos[p->archivo_actual];
            liberar_acceso_archivo(sched->gestor_archivos, nombre_archivo, p->pid);
        }

        // Liberar memoria
        if (sched->gestor_memoria)
        {
            liberar_memoria_proceso(sched->gestor_memoria, p->pid);
        }

        sched->procesos_completados++;
        sched->tiempo_espera_total += p->tiempo_espera;
        sched->tiempo_retorno_total += p->tiempo_retorno;
        sched->tiempo_respuesta_total += p->tiempo_respuesta;

        destruir_proceso(p);
        sched->proceso_actual = NULL;
    }
}

void actualizar_procesos_bloqueados(Planificador *sched)
{
    if (!sched || cola_vacia(&sched->cola_bloqueados))
        return;

    // Revisar procesos bloqueados y mover a listos si es posible
    int num_bloqueados = sched->cola_bloqueados.cantidad;

    for (int i = 0; i < num_bloqueados; i++)
    {
        Proceso *p = desencolar(&sched->cola_bloqueados);

        if (p->archivo_actual != -1 && sched->gestor_archivos)
        {
            char *nombre_archivo = p->archivos_abiertos[p->archivo_actual];
            ModoAcceso modo = (ModoAcceso)p->modo_acceso;

            int resultado = solicitar_acceso_archivo(sched->gestor_archivos,
                                                     nombre_archivo, p->pid, modo);

            if (resultado == 1)
            {
                // Acceso concedido
                p->estado = LISTO;
                encolar(&sched->cola_listos, p);
            }
            else
            {
                // Sigue bloqueado
                encolar(&sched->cola_bloqueados, p);
            }
        }
        else
        {
            // No hay razón para estar bloqueado
            p->estado = LISTO;
            encolar(&sched->cola_listos, p);
        }
    }
}

void ejecutar_ciclo(Planificador *sched)
{
    if (!sched)
        return;

    // Actualizar procesos bloqueados
    actualizar_procesos_bloqueados(sched);

    // Actualizar tiempo de espera de procesos en cola
    Proceso *p = sched->cola_listos.frente;
    while (p != NULL)
    {
        p->tiempo_espera++;
        p = p->siguiente;
    }

    // Si no hay proceso en ejecución, seleccionar uno
    if (sched->proceso_actual == NULL)
    {
        sched->proceso_actual = seleccionar_siguiente_proceso(sched);
        if (sched->proceso_actual != NULL)
        {
            cambio_contexto(sched, sched->proceso_actual);
        }
    }

    // Ejecutar proceso actual
    if (sched->proceso_actual != NULL)
    {
        int tiempo = (sched->algoritmo == ROUND_ROBIN) ? (sched->quantum < sched->proceso_actual->tiempo_restante ? sched->quantum : sched->proceso_actual->tiempo_restante) : sched->proceso_actual->tiempo_restante;

        simular_ejecucion_proceso(sched, tiempo);
    }
    else
    {
        // CPU ociosa
        sched->tiempo_actual++;
    }
}

void ejecutar_simulacion(Planificador *sched, int duracion_maxima)
{
    if (!sched)
        return;

    printf("\n=== INICIANDO SIMULACION ===\n");
    printf("Algoritmo: ");
    switch (sched->algoritmo)
    {
    case ROUND_ROBIN:
        printf("Round Robin (Quantum=%d)\n", sched->quantum);
        break;
    case SJF:
        printf("Shortest Job First\n");
        break;
    case PRIORIDAD:
        printf("Prioridad\n");
        break;
    }
    printf("Procesos: %d\n", sched->procesos_creados);
    printf("\n");

    sched->tiempo_inicio = sched->tiempo_actual;
    int ultimo_reporte = 0;

    while (sched->procesos_completados < sched->procesos_creados &&
           sched->tiempo_actual < duracion_maxima)
    {

        ejecutar_ciclo(sched);

        // Mostrar estado cada 20 unidades de tiempo
        if (sched->tiempo_actual - ultimo_reporte >= 20)
        {
            imprimir_estado_sistema(sched);
            ultimo_reporte = sched->tiempo_actual;
        }
    }

    printf("\n=== SIMULACION FINALIZADA ===\n");
    printf("Tiempo total: %d unidades\n", sched->tiempo_actual - sched->tiempo_inicio);
    printf("Procesos completados: %d/%d\n\n", sched->procesos_completados, sched->procesos_creados);
}

void imprimir_estado_sistema(Planificador *sched)
{
    if (!sched)
        return;

    printf("--- Tiempo: %d ---\n", sched->tiempo_actual);
    printf("  Completados: %d/%d\n", sched->procesos_completados, sched->procesos_creados);

    if (sched->proceso_actual)
    {
        printf("  Ejecutando: P%d (Restante: %d)\n",
               sched->proceso_actual->pid, sched->proceso_actual->tiempo_restante);
    }
    else
    {
        printf("  Ejecutando: [CPU OCIOSA]\n");
    }

    imprimir_cola(&sched->cola_listos, "Listos");
    imprimir_cola(&sched->cola_bloqueados, "Bloqueados");

    if (sched->gestor_memoria)
    {
        imprimir_estado_memoria(sched->gestor_memoria);
    }

    if (sched->gestor_archivos)
    {
        imprimir_estado_archivos(sched->gestor_archivos);
    }

    printf("\n");
}

void imprimir_metricas(Planificador *sched)
{
    if (!sched || sched->procesos_completados == 0)
        return;

    printf("\n=== METRICAS DE PLANIFICACION ===\n");

    double tiempo_espera_prom = sched->tiempo_espera_total / sched->procesos_completados;
    double tiempo_retorno_prom = sched->tiempo_retorno_total / sched->procesos_completados;
    double tiempo_respuesta_prom = sched->tiempo_respuesta_total / sched->procesos_completados;

    int tiempo_total = sched->tiempo_actual - sched->tiempo_inicio;
    double utilizacion = (tiempo_total > 0) ? (sched->tiempo_cpu_usado * 100.0) / tiempo_total : 0;

    double throughput = (tiempo_total > 0) ? (sched->procesos_completados * 1.0) / tiempo_total : 0;

    printf("Tiempo de espera promedio: %.2f unidades\n", tiempo_espera_prom);
    printf("Tiempo de retorno promedio: %.2f unidades\n", tiempo_retorno_prom);
    printf("Tiempo de respuesta promedio: %.2f unidades\n", tiempo_respuesta_prom);
    printf("Utilizacion de CPU: %.2f%%\n", utilizacion);
    printf("Throughput: %.4f procesos/unidad\n", throughput);
    printf("Cambios de contexto: %d\n", sched->cambios_contexto);
}
