#include "proceso.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Crear un nuevo proceso
Proceso *crear_proceso(int pid, const char *nombre, int prioridad, int tiempo_rafaga,
                       int num_paginas, int num_archivos)
{
    Proceso *p = (Proceso *)malloc(sizeof(Proceso));
    if (!p)
    {
        fprintf(stderr, "Error: No se pudo asignar memoria para el proceso\n");
        return NULL;
    }

    p->pid = pid;
    strncpy(p->nombre, nombre, 31);
    p->nombre[31] = '\0';
    p->estado = NUEVO;
    p->prioridad = prioridad;
    p->tiempo_llegada = 0;
    p->tiempo_rafaga = tiempo_rafaga;
    p->tiempo_restante = tiempo_rafaga;
    p->tiempo_espera = 0;
    p->tiempo_retorno = 0;
    p->tiempo_respuesta = -1; // -1 indica que no ha ejecutado aún
    p->quantum_restante = 0;

    // Inicializar gestión de memoria
    p->num_paginas = num_paginas;
    p->tamanio_memoria = num_paginas;
    p->paginas = (int *)malloc(sizeof(int) * num_paginas);
    for (int i = 0; i < num_paginas; i++)
    {
        p->paginas[i] = i; // Páginas numeradas secuencialmente
    }
    p->pagina_actual = 0;

    // Inicializar gestión de archivos
    p->num_archivos = num_archivos;
    p->archivos_abiertos = NULL;
    if (num_archivos > 0)
    {
        p->archivos_abiertos = (char **)malloc(sizeof(char *) * num_archivos);
        for (int i = 0; i < num_archivos; i++)
        {
            p->archivos_abiertos[i] = (char *)malloc(64);
        }
    }
    p->archivo_actual = -1;
    p->modo_acceso = 0;

    p->siguiente = NULL;

    return p;
}

// Destruir un proceso y liberar su memoria
void destruir_proceso(Proceso *p)
{
    if (!p)
        return;

    if (p->paginas)
    {
        free(p->paginas);
    }

    if (p->archivos_abiertos)
    {
        for (int i = 0; i < p->num_archivos; i++)
        {
            if (p->archivos_abiertos[i])
            {
                free(p->archivos_abiertos[i]);
            }
        }
        free(p->archivos_abiertos);
    }

    free(p);
}

// Convertir estado a cadena de texto
const char *estado_a_string(EstadoProceso estado)
{
    switch (estado)
    {
    case NUEVO:
        return "NUEVO";
    case LISTO:
        return "LISTO";
    case EJECUTANDO:
        return "EJECUTANDO";
    case BLOQUEADO:
        return "BLOQUEADO";
    case TERMINADO:
        return "TERMINADO";
    default:
        return "DESCONOCIDO";
    }
}

// Imprimir información de un proceso
void imprimir_proceso(Proceso *p)
{
    if (!p)
        return;

    printf("Proceso [PID=%d, Nombre=%s, Estado=%s, Prioridad=%d, "
           "Rafaga=%d, Restante=%d, Espera=%d, Paginas=%d, Archivos=%d]\n",
           p->pid, p->nombre, estado_a_string(p->estado), p->prioridad,
           p->tiempo_rafaga, p->tiempo_restante, p->tiempo_espera,
           p->num_paginas, p->num_archivos);
}

// Generar un proceso con características aleatorias
Proceso *generar_proceso_aleatorio(int pid)
{
    char nombre[32];
    sprintf(nombre, "P%d", pid);

    // Prioridad entre 0 (alta) y 9 (baja)
    int prioridad = rand() % 10;

    // Tiempo de ráfaga entre 5 y 30 unidades
    int tiempo_rafaga = 5 + (rand() % 26);

    // Número de páginas entre 3 y 12
    int num_paginas = 3 + (rand() % 10);

    // Número de archivos entre 0 y 3
    int num_archivos = rand() % 4;

    return crear_proceso(pid, nombre, prioridad, tiempo_rafaga, num_paginas, num_archivos);
}

// Asignar archivos aleatorios a un proceso
void asignar_archivos_proceso(Proceso *p, int num_archivos_sistema)
{
    if (!p || !p->archivos_abiertos || p->num_archivos == 0)
        return;

    for (int i = 0; i < p->num_archivos; i++)
    {
        int archivo_idx = rand() % num_archivos_sistema;
        sprintf(p->archivos_abiertos[i], "archivo_%d.dat", archivo_idx);
    }
}
