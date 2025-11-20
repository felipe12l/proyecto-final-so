#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "proceso.h"
#include "memoria.h"
#include "archivo.h"
#include "planificador.h"

// Configuración del simulador
typedef struct
{
    AlgoritmoScheduling algoritmo;
    int quantum;
    AlgoritmoReemplazo reemplazo;
    int num_marcos;
    int num_procesos;
    int num_archivos;
    int duracion_maxima;
} Configuracion;

// Valores por defecto
void inicializar_configuracion(Configuracion *config)
{
    config->algoritmo = ROUND_ROBIN;
    config->quantum = 4;
    config->reemplazo = LRU;
    config->num_marcos = 32;
    config->num_procesos = 10;
    config->num_archivos = 5;
    config->duracion_maxima = 500;
}

// Mostrar menú principal
void mostrar_menu()
{
    printf("\n");
    printf("╔════════════════════════════════════════╗\n");
    printf("║  SIMULADOR DE SISTEMA OPERATIVO       ║\n");
    printf("╚════════════════════════════════════════╝\n");
    printf("\n");
    printf("1. Configurar simulacion\n");
    printf("2. Ejecutar simulacion\n");
    printf("3. Ejecutar simulacion rapida (default)\n");
    printf("4. Comparar algoritmos\n");
    printf("5. Salir\n");
    printf("\n");
    printf("Seleccione una opcion: ");
}

// Configurar la simulación
void configurar_simulacion(Configuracion *config)
{
    int opcion;

    printf("\n=== CONFIGURACION ===\n\n");

    // Algoritmo de planificación
    printf("Algoritmo de planificacion:\n");
    printf("  1. Round Robin\n");
    printf("  2. Shortest Job First (SJF)\n");
    printf("  3. Prioridad\n");
    printf("Seleccione (1-3): ");
    scanf("%d", &opcion);

    switch (opcion)
    {
    case 1:
        config->algoritmo = ROUND_ROBIN;
        break;
    case 2:
        config->algoritmo = SJF;
        break;
    case 3:
        config->algoritmo = PRIORIDAD;
        break;
    default:
        printf("Opcion invalida, usando Round Robin\n");
    }

    // Quantum (solo para Round Robin)
    if (config->algoritmo == ROUND_ROBIN)
    {
        printf("Quantum (1-10): ");
        scanf("%d", &config->quantum);
        if (config->quantum < 1 || config->quantum > 10)
        {
            config->quantum = 4;
            printf("Valor invalido, usando quantum = 4\n");
        }
    }

    // Algoritmo de reemplazo
    printf("\nAlgoritmo de reemplazo de paginas:\n");
    printf("  1. FIFO\n");
    printf("  2. LRU\n");
    printf("Seleccione (1-2): ");
    scanf("%d", &opcion);
    config->reemplazo = (opcion == 1) ? FIFO : LRU;

    // Número de marcos
    printf("\nNumero de marcos de memoria (16-128): ");
    scanf("%d", &config->num_marcos);
    if (config->num_marcos < 16 || config->num_marcos > 128)
    {
        config->num_marcos = 32;
        printf("Valor invalido, usando 32 marcos\n");
    }

    // Número de procesos
    printf("Numero de procesos (5-50): ");
    scanf("%d", &config->num_procesos);
    if (config->num_procesos < 5 || config->num_procesos > 50)
    {
        config->num_procesos = 10;
        printf("Valor invalido, usando 10 procesos\n");
    }

    // Número de archivos
    printf("Numero de archivos del sistema (3-20): ");
    scanf("%d", &config->num_archivos);
    if (config->num_archivos < 3 || config->num_archivos > 20)
    {
        config->num_archivos = 5;
        printf("Valor invalido, usando 5 archivos\n");
    }

    printf("\nConfiguracion guardada!\n");
}

// Mostrar configuración actual
void mostrar_configuracion(Configuracion *config)
{
    printf("\n=== CONFIGURACION ACTUAL ===\n");
    printf("Algoritmo de planificacion: ");
    switch (config->algoritmo)
    {
    case ROUND_ROBIN:
        printf("Round Robin (Quantum=%d)\n", config->quantum);
        break;
    case SJF:
        printf("Shortest Job First\n");
        break;
    case PRIORIDAD:
        printf("Prioridad\n");
        break;
    }
    printf("Algoritmo de reemplazo: %s\n", config->reemplazo == FIFO ? "FIFO" : "LRU");
    printf("Marcos de memoria: %d\n", config->num_marcos);
    printf("Numero de procesos: %d\n", config->num_procesos);
    printf("Numero de archivos: %d\n", config->num_archivos);
    printf("Duracion maxima: %d unidades\n", config->duracion_maxima);
}

// Ejecutar una simulación
void ejecutar_con_configuracion(Configuracion *config)
{
    mostrar_configuracion(config);

    printf("\nPresione Enter para iniciar...");
    getchar();
    getchar();

    // Crear componentes del sistema
    GestorMemoria *gm = crear_gestor_memoria(config->num_marcos, config->reemplazo);
    GestorArchivos *ga = crear_gestor_archivos(config->num_archivos);
    Planificador *sched = crear_planificador(config->algoritmo, config->quantum, gm, ga);

    if (!gm || !ga || !sched)
    {
        fprintf(stderr, "Error al crear componentes del sistema\n");
        if (gm)
            destruir_gestor_memoria(gm);
        if (ga)
            destruir_gestor_archivos(ga);
        if (sched)
            destruir_planificador(sched);
        return;
    }

    // Generar procesos
    printf("\nGenerando %d procesos...\n", config->num_procesos);
    for (int i = 0; i < config->num_procesos; i++)
    {
        Proceso *p = generar_proceso_aleatorio(i);
        if (p)
        {
            asignar_archivos_proceso(p, config->num_archivos);
            agregar_proceso(sched, p);
        }
    }

    // Ejecutar simulación
    ejecutar_simulacion(sched, config->duracion_maxima);

    // Mostrar estadísticas finales
    imprimir_metricas(sched);

    if (gm)
    {
        imprimir_estadisticas_memoria(gm);
    }

    if (ga)
    {
        imprimir_estadisticas_archivos(ga);
    }

    // Liberar recursos
    destruir_planificador(sched);
    destruir_gestor_memoria(gm);
    destruir_gestor_archivos(ga);

    printf("\nPresione Enter para continuar...");
    getchar();
}

// Ejecutar simulación rápida con valores por defecto
void ejecutar_simulacion_rapida()
{
    Configuracion config;
    inicializar_configuracion(&config);
    config.num_procesos = 8;
    config.duracion_maxima = 300;

    printf("\n=== SIMULACION RAPIDA ===\n");
    printf("Usando configuracion por defecto\n");

    ejecutar_con_configuracion(&config);
}

// Comparar diferentes algoritmos de planificación
void comparar_algoritmos()
{
    printf("\n=== COMPARACION DE ALGORITMOS ===\n");
    printf("Se ejecutaran 3 simulaciones con los mismos procesos\n");
    printf("pero diferentes algoritmos de planificacion.\n");
    printf("\nPresione Enter para iniciar...");
    getchar();
    getchar();

    // Configuración base
    Configuracion config;
    inicializar_configuracion(&config);
    config.num_procesos = 10;
    config.duracion_maxima = 400;

    // Guardar semilla para reproducir procesos
    unsigned int semilla = (unsigned int)time(NULL);

    AlgoritmoScheduling algoritmos[] = {ROUND_ROBIN, SJF, PRIORIDAD};
    const char *nombres[] = {"Round Robin", "Shortest Job First", "Prioridad"};

    double tiempos_espera[3];
    double tiempos_retorno[3];
    double utilizacion_cpu[3];

    for (int i = 0; i < 3; i++)
    {
        printf("\n\n========================================\n");
        printf("PROBANDO: %s\n", nombres[i]);
        printf("========================================\n");

        config.algoritmo = algoritmos[i];
        srand(semilla); // Usar misma semilla para reproducibilidad

        GestorMemoria *gm = crear_gestor_memoria(config.num_marcos, config.reemplazo);
        GestorArchivos *ga = crear_gestor_archivos(config.num_archivos);
        Planificador *sched = crear_planificador(config.algoritmo, config.quantum, gm, ga);

        // Generar procesos
        for (int j = 0; j < config.num_procesos; j++)
        {
            Proceso *p = generar_proceso_aleatorio(j);
            if (p)
            {
                asignar_archivos_proceso(p, config.num_archivos);
                agregar_proceso(sched, p);
            }
        }

        // Ejecutar
        ejecutar_simulacion(sched, config.duracion_maxima);

        // Recolectar métricas
        if (sched->procesos_completados > 0)
        {
            tiempos_espera[i] = sched->tiempo_espera_total / sched->procesos_completados;
            tiempos_retorno[i] = sched->tiempo_retorno_total / sched->procesos_completados;
            int tiempo_total = sched->tiempo_actual - sched->tiempo_inicio;
            utilizacion_cpu[i] = (tiempo_total > 0) ? (sched->tiempo_cpu_usado * 100.0) / tiempo_total : 0;
        }

        imprimir_metricas(sched);

        // Liberar
        destruir_planificador(sched);
        destruir_gestor_memoria(gm);
        destruir_gestor_archivos(ga);
    }

    // Resumen comparativo
    printf("\n\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║              RESUMEN COMPARATIVO                              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    printf("%-20s %15s %15s %15s\n", "Algoritmo", "T. Espera", "T. Retorno", "Util. CPU");
    printf("─────────────────────────────────────────────────────────────────\n");

    for (int i = 0; i < 3; i++)
    {
        printf("%-20s %14.2f %14.2f %14.2f%%\n",
               nombres[i], tiempos_espera[i], tiempos_retorno[i], utilizacion_cpu[i]);
    }

    printf("\nPresione Enter para continuar...");
    getchar();
}

// Programa principal
int main(int argc, char *argv[])
{
    // Inicializar generador de números aleatorios
    srand((unsigned int)time(NULL));

    // Verificar argumentos de línea de comandos
    if (argc > 1)
    {
        Configuracion config;
        inicializar_configuracion(&config);

        for (int i = 1; i < argc; i++)
        {
            if (strcmp(argv[i], "-a") == 0 && i + 1 < argc)
            {
                i++;
                if (strcmp(argv[i], "rr") == 0)
                    config.algoritmo = ROUND_ROBIN;
                else if (strcmp(argv[i], "sjf") == 0)
                    config.algoritmo = SJF;
                else if (strcmp(argv[i], "prioridad") == 0)
                    config.algoritmo = PRIORIDAD;
            }
            else if (strcmp(argv[i], "-q") == 0 && i + 1 < argc)
            {
                config.quantum = atoi(argv[++i]);
            }
            else if (strcmp(argv[i], "-r") == 0 && i + 1 < argc)
            {
                i++;
                config.reemplazo = (strcmp(argv[i], "fifo") == 0) ? FIFO : LRU;
            }
            else if (strcmp(argv[i], "-m") == 0 && i + 1 < argc)
            {
                config.num_marcos = atoi(argv[++i]);
            }
            else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc)
            {
                config.num_procesos = atoi(argv[++i]);
            }
            else if (strcmp(argv[i], "-f") == 0 && i + 1 < argc)
            {
                config.num_archivos = atoi(argv[++i]);
            }
        }

        ejecutar_con_configuracion(&config);
        return 0;
    }

    // Modo interactivo
    Configuracion config;
    inicializar_configuracion(&config);
    int opcion;
    int continuar = 1;

    while (continuar)
    {
        mostrar_menu();
        scanf("%d", &opcion);

        switch (opcion)
        {
        case 1:
            configurar_simulacion(&config);
            break;

        case 2:
            ejecutar_con_configuracion(&config);
            break;

        case 3:
            ejecutar_simulacion_rapida();
            break;

        case 4:
            comparar_algoritmos();
            break;

        case 5:
            printf("\n¡Hasta luego!\n");
            continuar = 0;
            break;

        default:
            printf("\nOpcion invalida. Intente de nuevo.\n");
        }
    }

    return 0;
}
