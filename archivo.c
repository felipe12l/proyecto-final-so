#include "archivo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Crear gestor de archivos
GestorArchivos *crear_gestor_archivos(int num_archivos)
{
    GestorArchivos *ga = (GestorArchivos *)malloc(sizeof(GestorArchivos));
    if (!ga)
    {
        fprintf(stderr, "Error: No se pudo asignar memoria para gestor de archivos\n");
        return NULL;
    }

    ga->num_archivos = num_archivos < MAX_ARCHIVOS ? num_archivos : MAX_ARCHIVOS;

    // Inicializar archivos
    for (int i = 0; i < ga->num_archivos; i++)
    {
        sprintf(ga->archivos[i].nombre, "archivo_%d.dat", i);
        ga->archivos[i].tamanio = 1024 + (rand() % 9216); // 1KB - 10KB
        ga->archivos[i].bloqueo.pid = -1;
        ga->archivos[i].bloqueo.modo = LECTURA;
        ga->archivos[i].bloqueo.activo = 0;
        ga->archivos[i].lectores_activos = 0;
        ga->archivos[i].escritor_activo = 0;
        ga->archivos[i].num_esperando = 0;

        for (int j = 0; j < MAX_ESPERA; j++)
        {
            ga->archivos[i].cola_espera[j] = -1;
        }
    }

    ga->num_conflictos = 0;
    ga->num_accesos_exitosos = 0;
    ga->num_accesos_lectura = 0;
    ga->num_accesos_escritura = 0;
    ga->num_bloqueos_resueltos = 0;

    return ga;
}

// Destruir gestor de archivos
void destruir_gestor_archivos(GestorArchivos *ga)
{
    if (ga)
    {
        free(ga);
    }
}

// Buscar un archivo por nombre
int buscar_archivo(GestorArchivos *ga, const char *nombre)
{
    if (!ga || !nombre)
        return -1;

    for (int i = 0; i < ga->num_archivos; i++)
    {
        if (strcmp(ga->archivos[i].nombre, nombre) == 0)
        {
            return i;
        }
    }
    return -1;
}

// Verificar si hay conflicto con el acceso solicitado
int verificar_conflicto(Archivo *archivo, ModoAcceso modo)
{
    if (modo == ESCRITURA)
    {
        // Escritor necesita acceso exclusivo
        return (archivo->lectores_activos > 0 || archivo->escritor_activo);
    }
    else
    { // LECTURA
        // Lector solo conflictúa con escritor
        return archivo->escritor_activo;
    }
}

// Solicitar acceso a un archivo
int solicitar_acceso_archivo(GestorArchivos *ga, const char *nombre, int pid, ModoAcceso modo)
{
    if (!ga || !nombre)
        return -1;

    int idx = buscar_archivo(ga, nombre);
    if (idx == -1)
        return -1;

    Archivo *archivo = &ga->archivos[idx];

    // Verificar conflicto
    if (verificar_conflicto(archivo, modo))
    {
        // Hay conflicto, agregar a cola de espera
        if (archivo->num_esperando < MAX_ESPERA)
        {
            archivo->cola_espera[archivo->num_esperando] = pid;
            archivo->num_esperando++;
            ga->num_conflictos++;
            return 0; // Bloqueado
        }
        else
        {
            return -1; // Cola llena, error
        }
    }

    // No hay conflicto, conceder acceso
    if (modo == ESCRITURA)
    {
        archivo->escritor_activo = 1;
        archivo->bloqueo.pid = pid;
        archivo->bloqueo.modo = ESCRITURA;
        archivo->bloqueo.activo = 1;
        ga->num_accesos_escritura++;
    }
    else
    { // LECTURA
        archivo->lectores_activos++;
        if (archivo->lectores_activos == 1)
        {
            archivo->bloqueo.pid = pid;
            archivo->bloqueo.modo = LECTURA;
            archivo->bloqueo.activo = 1;
        }
        ga->num_accesos_lectura++;
    }

    ga->num_accesos_exitosos++;
    return 1; // Acceso concedido
}

// Liberar acceso a un archivo
int liberar_acceso_archivo(GestorArchivos *ga, const char *nombre, int pid)
{
    if (!ga || !nombre)
        return -1;

    int idx = buscar_archivo(ga, nombre);
    if (idx == -1)
        return -1;

    Archivo *archivo = &ga->archivos[idx];

    // Liberar según el tipo de acceso
    if (archivo->escritor_activo && archivo->bloqueo.pid == pid)
    {
        archivo->escritor_activo = 0;
        archivo->bloqueo.activo = 0;
        archivo->bloqueo.pid = -1;
    }
    else if (archivo->lectores_activos > 0)
    {
        archivo->lectores_activos--;
        if (archivo->lectores_activos == 0)
        {
            archivo->bloqueo.activo = 0;
            archivo->bloqueo.pid = -1;
        }
    }

    // Procesar cola de espera
    procesar_cola_espera(ga, idx);

    return 1;
}

// Procesar cola de espera de un archivo
void procesar_cola_espera(GestorArchivos *ga, int indice_archivo)
{
    if (!ga || indice_archivo < 0 || indice_archivo >= ga->num_archivos)
        return;

    Archivo *archivo = &ga->archivos[indice_archivo];

    if (archivo->num_esperando == 0)
        return;

    // Intentar conceder acceso al primero en cola
    // (Simplificado: solo procesamos uno a la vez)
    int pid_esperando = archivo->cola_espera[0];

    // Eliminar de la cola
    for (int i = 0; i < archivo->num_esperando - 1; i++)
    {
        archivo->cola_espera[i] = archivo->cola_espera[i + 1];
    }
    archivo->num_esperando--;
    archivo->cola_espera[archivo->num_esperando] = -1;

    ga->num_bloqueos_resueltos++;
}

// Imprimir estado de archivos
void imprimir_estado_archivos(GestorArchivos *ga)
{
    if (!ga)
        return;

    int archivos_bloqueados = 0;
    for (int i = 0; i < ga->num_archivos; i++)
    {
        if (ga->archivos[i].bloqueo.activo)
        {
            archivos_bloqueados++;
        }
    }

    printf("  [ARCHIVOS] Bloqueados: %d/%d", archivos_bloqueados, ga->num_archivos);
    if (ga->num_conflictos > 0)
    {
        printf(", Conflictos: %d", ga->num_conflictos);
    }
    printf("\n");
}

// Imprimir estadísticas de archivos
void imprimir_estadisticas_archivos(GestorArchivos *ga)
{
    if (!ga)
        return;

    printf("\n=== ESTADISTICAS DE ARCHIVOS ===\n");
    printf("Total de archivos: %d\n", ga->num_archivos);
    printf("Accesos exitosos: %d\n", ga->num_accesos_exitosos);
    printf("  - Lectura: %d\n", ga->num_accesos_lectura);
    printf("  - Escritura: %d\n", ga->num_accesos_escritura);
    printf("Conflictos detectados: %d\n", ga->num_conflictos);
    printf("Bloqueos resueltos: %d\n", ga->num_bloqueos_resueltos);

    if (ga->num_accesos_exitosos > 0)
    {
        printf("Tasa de conflicto: %.2f%%\n",
               (ga->num_conflictos * 100.0) / (ga->num_accesos_exitosos + ga->num_conflictos));
    }
}
