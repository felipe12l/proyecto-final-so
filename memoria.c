#include "memoria.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Crear gestor de memoria
GestorMemoria *crear_gestor_memoria(int num_marcos, AlgoritmoReemplazo algoritmo)
{
    GestorMemoria *gm = (GestorMemoria *)malloc(sizeof(GestorMemoria));
    if (!gm)
    {
        fprintf(stderr, "Error: No se pudo asignar memoria para el gestor\n");
        return NULL;
    }

    gm->num_marcos = num_marcos;
    gm->marcos = (MarcoPagina *)calloc(num_marcos, sizeof(MarcoPagina));
    if (!gm->marcos)
    {
        free(gm);
        return NULL;
    }

    // Inicializar marcos
    for (int i = 0; i < num_marcos; i++)
    {
        gm->marcos[i].ocupado = 0;
        gm->marcos[i].pid = -1;
        gm->marcos[i].num_pagina = -1;
        gm->marcos[i].tiempo_carga = 0;
        gm->marcos[i].ultimo_acceso = 0;
        gm->marcos[i].bit_referencia = 0;
        gm->marcos[i].bit_modificado = 0;
    }

    gm->capacidad_procesos = 10;
    gm->tablas_paginas = (TablaPaginas *)calloc(gm->capacidad_procesos, sizeof(TablaPaginas));
    gm->num_procesos = 0;
    gm->tiempo_actual = 0;
    gm->fallos_pagina = 0;
    gm->num_reemplazos = 0;
    gm->num_accesos = 0;
    gm->algoritmo = algoritmo;

    // Inicializar cola FIFO
    gm->cola_fifo = (int *)malloc(sizeof(int) * num_marcos);
    gm->frente_fifo = 0;
    gm->final_fifo = 0;

    return gm;
}

// Destruir gestor de memoria
void destruir_gestor_memoria(GestorMemoria *gm)
{
    if (!gm)
        return;

    if (gm->marcos)
        free(gm->marcos);
    if (gm->cola_fifo)
        free(gm->cola_fifo);

    if (gm->tablas_paginas)
    {
        for (int i = 0; i < gm->num_procesos; i++)
        {
            if (gm->tablas_paginas[i].marco_fisico)
            {
                free(gm->tablas_paginas[i].marco_fisico);
            }
            if (gm->tablas_paginas[i].valido)
            {
                free(gm->tablas_paginas[i].valido);
            }
        }
        free(gm->tablas_paginas);
    }

    free(gm);
}

// Registrar un nuevo proceso en el gestor de memoria
void registrar_proceso_memoria(GestorMemoria *gm, Proceso *p)
{
    if (!gm || !p)
        return;

    if (gm->num_procesos >= gm->capacidad_procesos)
    {
        // Expandir capacidad
        gm->capacidad_procesos *= 2;
        gm->tablas_paginas = (TablaPaginas *)realloc(gm->tablas_paginas,
                                                     gm->capacidad_procesos * sizeof(TablaPaginas));
    }

    TablaPaginas *tp = &gm->tablas_paginas[gm->num_procesos];
    tp->pid = p->pid;
    tp->num_paginas = p->num_paginas;
    tp->marco_fisico = (int *)malloc(sizeof(int) * p->num_paginas);
    tp->valido = (int *)calloc(p->num_paginas, sizeof(int));

    // Inicializar todas las páginas como no cargadas
    for (int i = 0; i < p->num_paginas; i++)
    {
        tp->marco_fisico[i] = -1;
        tp->valido[i] = 0;
    }

    gm->num_procesos++;
}

// Buscar tabla de páginas de un proceso
static TablaPaginas *buscar_tabla_paginas(GestorMemoria *gm, int pid)
{
    for (int i = 0; i < gm->num_procesos; i++)
    {
        if (gm->tablas_paginas[i].pid == pid)
        {
            return &gm->tablas_paginas[i];
        }
    }
    return NULL;
}

// Buscar marco libre
static int buscar_marco_libre(GestorMemoria *gm)
{
    for (int i = 0; i < gm->num_marcos; i++)
    {
        if (!gm->marcos[i].ocupado)
        {
            return i;
        }
    }
    return -1;
}

// Encontrar marco víctima usando FIFO
int encontrar_marco_fifo(GestorMemoria *gm)
{
    if (gm->frente_fifo == gm->final_fifo && gm->marcos[gm->cola_fifo[gm->frente_fifo]].ocupado == 0)
    {
        return -1; // Cola vacía
    }

    int marco = gm->cola_fifo[gm->frente_fifo];
    gm->frente_fifo = (gm->frente_fifo + 1) % gm->num_marcos;
    return marco;
}

// Encontrar marco víctima usando LRU
int encontrar_marco_lru(GestorMemoria *gm)
{
    int marco_lru = -1;
    int tiempo_min = gm->tiempo_actual + 1;

    for (int i = 0; i < gm->num_marcos; i++)
    {
        if (gm->marcos[i].ocupado && gm->marcos[i].ultimo_acceso < tiempo_min)
        {
            tiempo_min = gm->marcos[i].ultimo_acceso;
            marco_lru = i;
        }
    }

    return marco_lru;
}

// Reemplazar una página
int reemplazar_pagina(GestorMemoria *gm, int pid, int num_pagina, int tiempo)
{
    int marco_victima;

    if (gm->algoritmo == FIFO)
    {
        marco_victima = encontrar_marco_fifo(gm);
    }
    else
    { // LRU
        marco_victima = encontrar_marco_lru(gm);
    }

    if (marco_victima == -1)
    {
        return -1;
    }

    // Actualizar tabla del proceso víctima
    int pid_victima = gm->marcos[marco_victima].pid;
    int pagina_victima = gm->marcos[marco_victima].num_pagina;

    TablaPaginas *tp_victima = buscar_tabla_paginas(gm, pid_victima);
    if (tp_victima && pagina_victima >= 0 && pagina_victima < tp_victima->num_paginas)
    {
        tp_victima->marco_fisico[pagina_victima] = -1;
        tp_victima->valido[pagina_victima] = 0;
    }

    // Cargar nueva página
    gm->marcos[marco_victima].pid = pid;
    gm->marcos[marco_victima].num_pagina = num_pagina;
    gm->marcos[marco_victima].tiempo_carga = tiempo;
    gm->marcos[marco_victima].ultimo_acceso = tiempo;
    gm->marcos[marco_victima].bit_referencia = 1;
    gm->marcos[marco_victima].bit_modificado = 0;

    gm->num_reemplazos++;

    return marco_victima;
}

// Acceder a una página (retorna 1 si es fallo, 0 si hit)
int acceder_pagina(GestorMemoria *gm, int pid, int num_pagina, int tiempo)
{
    if (!gm)
        return -1;

    gm->num_accesos++;
    gm->tiempo_actual = tiempo;

    TablaPaginas *tp = buscar_tabla_paginas(gm, pid);
    if (!tp || num_pagina < 0 || num_pagina >= tp->num_paginas)
    {
        return -1; // Error
    }

    // Verificar si la página ya está en memoria
    if (tp->valido[num_pagina])
    {
        int marco = tp->marco_fisico[num_pagina];
        gm->marcos[marco].ultimo_acceso = tiempo;
        gm->marcos[marco].bit_referencia = 1;
        return 0; // Hit
    }

    // Fallo de página
    gm->fallos_pagina++;

    // Buscar marco libre
    int marco = buscar_marco_libre(gm);

    if (marco == -1)
    {
        // No hay marcos libres, necesita reemplazo
        marco = reemplazar_pagina(gm, pid, num_pagina, tiempo);
        if (marco == -1)
        {
            return -1; // Error en reemplazo
        }
    }
    else
    {
        // Cargar en marco libre
        gm->marcos[marco].ocupado = 1;
        gm->marcos[marco].pid = pid;
        gm->marcos[marco].num_pagina = num_pagina;
        gm->marcos[marco].tiempo_carga = tiempo;
        gm->marcos[marco].ultimo_acceso = tiempo;
        gm->marcos[marco].bit_referencia = 1;
        gm->marcos[marco].bit_modificado = 0;

        // Agregar a cola FIFO
        gm->cola_fifo[gm->final_fifo] = marco;
        gm->final_fifo = (gm->final_fifo + 1) % gm->num_marcos;
    }

    // Actualizar tabla de páginas
    tp->marco_fisico[num_pagina] = marco;
    tp->valido[num_pagina] = 1;

    return 1; // Fallo
}

// Liberar memoria de un proceso
void liberar_memoria_proceso(GestorMemoria *gm, int pid)
{
    if (!gm)
        return;

    // Liberar marcos
    for (int i = 0; i < gm->num_marcos; i++)
    {
        if (gm->marcos[i].ocupado && gm->marcos[i].pid == pid)
        {
            gm->marcos[i].ocupado = 0;
            gm->marcos[i].pid = -1;
            gm->marcos[i].num_pagina = -1;
        }
    }
}

// Imprimir estado de la memoria
void imprimir_estado_memoria(GestorMemoria *gm)
{
    if (!gm)
        return;

    int marcos_ocupados = 0;
    for (int i = 0; i < gm->num_marcos; i++)
    {
        if (gm->marcos[i].ocupado)
            marcos_ocupados++;
    }

    printf("  [MEMORIA] Marcos: %d/%d ocupados (%.1f%%)\n",
           marcos_ocupados, gm->num_marcos,
           (marcos_ocupados * 100.0) / gm->num_marcos);
}

// Imprimir estadísticas de memoria
void imprimir_estadisticas_memoria(GestorMemoria *gm)
{
    if (!gm)
        return;

    printf("\n=== ESTADISTICAS DE MEMORIA ===\n");
    printf("Algoritmo de reemplazo: %s\n", gm->algoritmo == FIFO ? "FIFO" : "LRU");
    printf("Total de accesos: %d\n", gm->num_accesos);
    printf("Fallos de pagina: %d\n", gm->fallos_pagina);
    printf("Numero de reemplazos: %d\n", gm->num_reemplazos);

    if (gm->num_accesos > 0)
    {
        double tasa_fallos = (gm->fallos_pagina * 100.0) / gm->num_accesos;
        double hit_ratio = 100.0 - tasa_fallos;
        printf("Tasa de fallos: %.2f%%\n", tasa_fallos);
        printf("Hit ratio: %.2f%%\n", hit_ratio);
    }

    int marcos_ocupados = 0;
    for (int i = 0; i < gm->num_marcos; i++)
    {
        if (gm->marcos[i].ocupado)
            marcos_ocupados++;
    }
    printf("Marcos ocupados al final: %d/%d (%.1f%%)\n",
           marcos_ocupados, gm->num_marcos,
           (marcos_ocupados * 100.0) / gm->num_marcos);
}
