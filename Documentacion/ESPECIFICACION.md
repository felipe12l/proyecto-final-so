# Especificación del Simulador de Sistema Operativo

## 1. Definición del Problema y Contexto

### 1.1 Problema

Desarrollar un simulador de sistema operativo que permita comprender y demostrar los conceptos fundamentales de:

- **Planificación de procesos**: Gestión eficiente del tiempo de CPU entre múltiples procesos
- **Gestión de memoria**: Asignación dinámica mediante paginación por demanda
- **Gestión de archivos**: Control de acceso concurrente con mecanismos de sincronización

### 1.2 Contexto

Este simulador es una herramienta educativa que permite visualizar y analizar el comportamiento de un sistema operativo sin necesidad de modificar un kernel real. Se ejecuta completamente en espacio de usuario y simula las operaciones mediante estructuras de datos y algoritmos.

### 1.3 Objetivos

- Implementar múltiples algoritmos de planificación (Round Robin, SJF, Prioridad)
- Simular gestión de memoria virtual con paginación por demanda
- Implementar algoritmos de reemplazo de páginas (FIFO, LRU)
- Gestionar acceso concurrente a archivos con mecanismos de bloqueo
- Recolectar y analizar métricas de rendimiento

## 2. Diseño de Estructuras de Datos

### 2.1 Estructura de Proceso (PCB - Process Control Block)

```c
typedef enum {
    NUEVO,
    LISTO,
    EJECUTANDO,
    BLOQUEADO,
    TERMINADO
} EstadoProceso;

typedef struct Proceso {
    int pid;                    // Identificador único del proceso
    char nombre[32];            // Nombre descriptivo
    EstadoProceso estado;       // Estado actual
    int prioridad;              // Prioridad (0-10, menor = mayor prioridad)
    int tiempo_llegada;         // Tiempo de llegada a la cola
    int tiempo_rafaga;          // Tiempo total de CPU necesario
    int tiempo_restante;        // Tiempo que falta por ejecutar
    int tiempo_espera;          // Tiempo total esperando en cola
    int tiempo_retorno;         // Tiempo desde llegada hasta finalización
    int quantum_restante;       // Quantum restante (para Round Robin)

    // Gestión de memoria
    int tamanio_memoria;        // Tamaño en páginas
    int *paginas;               // Array de páginas asignadas
    int num_paginas;            // Número de páginas del proceso

    // Gestión de archivos
    char **archivos_abiertos;   // Lista de archivos que usa
    int num_archivos;           // Cantidad de archivos

    struct Proceso *siguiente;  // Para lista enlazada
} Proceso;
```

### 2.2 Estructura de Memoria

```c
#define TAM_PAGINA 4096         // 4KB por página
#define NUM_MARCOS 64           // 64 marcos de página (256KB total)

typedef struct MarcoPagina {
    int ocupado;                // 0 = libre, 1 = ocupado
    int pid;                    // ID del proceso dueño
    int num_pagina;             // Número de página lógica
    int tiempo_carga;           // Timestamp de cuándo se cargó
    int ultimo_acceso;          // Timestamp del último acceso (para LRU)
    int bit_referencia;         // Bit de referencia (para algoritmos)
    int bit_modificado;         // Bit de modificación (dirty bit)
} MarcoPagina;

typedef struct TablaPaginas {
    int pid;                    // Proceso dueño
    int num_paginas;            // Páginas totales del proceso
    int *marco_fisico;          // Array: índice=página lógica, valor=marco físico (-1 si no está)
    int *valido;                // Bit de validez para cada página
} TablaPaginas;

typedef struct GestorMemoria {
    MarcoPagina marcos[NUM_MARCOS];
    TablaPaginas *tablas_paginas;  // Array de tablas, una por proceso
    int num_procesos;
    int tiempo_actual;
    int fallos_pagina;
    int num_reemplazos;

    // Cola FIFO para reemplazo
    int cola_fifo[NUM_MARCOS];
    int frente_fifo;
    int final_fifo;
} GestorMemoria;
```

### 2.3 Estructura de Archivos

```c
typedef enum {
    LECTURA,
    ESCRITURA,
    LECTURA_ESCRITURA
} ModoAcceso;

typedef struct BloqueoArchivo {
    int pid;                    // Proceso que tiene el bloqueo
    ModoAcceso modo;            // Tipo de bloqueo
    int activo;                 // 1 = bloqueado, 0 = libre
} BloqueoArchivo;

typedef struct Archivo {
    char nombre[64];            // Nombre del archivo
    int tamanio;                // Tamaño en bytes
    BloqueoArchivo bloqueo;     // Estado de bloqueo
    int lectores_activos;       // Contador de lectores (para lectores-escritores)
    int escritor_activo;        // 1 si hay escritor, 0 si no

    // Cola de procesos esperando acceso
    int *cola_espera;           // Array de PIDs
    int num_esperando;          // Cantidad en espera
} Archivo;

typedef struct GestorArchivos {
    Archivo *archivos;
    int num_archivos;
    int num_conflictos;         // Contador de conflictos
    int num_accesos_exitosos;
} GestorArchivos;
```

### 2.4 Estructura del Planificador

```c
typedef enum {
    ROUND_ROBIN,
    SJF,                        // Shortest Job First
    PRIORIDAD
} AlgoritmoScheduling;

typedef struct ColaProcesos {
    Proceso *frente;
    Proceso *final;
    int cantidad;
} ColaProcesos;

typedef struct Planificador {
    AlgoritmoScheduling algoritmo;
    ColaProcesos cola_listos;
    ColaProcesos cola_bloqueados;
    Proceso *proceso_actual;
    int quantum;                // Para Round Robin
    int tiempo_actual;

    // Métricas
    int procesos_completados;
    double tiempo_espera_promedio;
    double tiempo_retorno_promedio;
    double utilizacion_cpu;
    int cambios_contexto;
} Planificador;
```

## 3. Diseño de Algoritmos

### 3.1 Algoritmos de Planificación

#### Round Robin

- Quantum configurable (por defecto 4 unidades de tiempo)
- Cola circular FIFO
- Proceso ejecuta hasta agotar quantum o terminar
- Si no termina, va al final de la cola

#### Shortest Job First (SJF)

- Selecciona proceso con menor tiempo de ráfaga restante
- No apropiativo (ejecuta hasta terminar o bloquearse)
- Minimiza tiempo de espera promedio

#### Prioridad

- Selecciona proceso con mayor prioridad (menor número)
- Puede ser apropiativo o no apropiativo
- Riesgo de inanición en procesos de baja prioridad

### 3.2 Algoritmos de Reemplazo de Páginas

#### FIFO (First In, First Out)

- Reemplaza la página más antigua en memoria
- Implementación con cola circular
- Simple pero puede sufrir de Anomalía de Belady

#### LRU (Least Recently Used)

- Reemplaza la página no usada hace más tiempo
- Usa timestamp del último acceso
- Mejor rendimiento pero más complejo

### 3.3 Gestión de Archivos

#### Bloqueo de Lectores-Escritores

- Múltiples lectores simultáneos permitidos
- Escritor tiene acceso exclusivo
- Cola de espera para procesos bloqueados
- Prioridad configurable (lectores vs escritores)

## 4. Flujo de Ejecución del Simulador

### 4.1 Inicialización

1. Crear gestor de memoria
2. Crear gestor de archivos
3. Crear planificador con algoritmo seleccionado
4. Generar procesos iniciales

### 4.2 Ciclo Principal

```
MIENTRAS haya procesos activos:
    1. Seleccionar siguiente proceso según algoritmo
    2. Simular ejecución:
       - Si accede a memoria → verificar/cargar página
       - Si accede a archivo → verificar/obtener bloqueo
       - Ejecutar por quantum o hasta bloquearse
    3. Actualizar estado del proceso
    4. Actualizar métricas
    5. Avanzar reloj del sistema
```

### 4.3 Finalización

1. Mostrar métricas de planificación
2. Mostrar estadísticas de memoria
3. Mostrar estadísticas de archivos
4. Generar reporte de ejecución

## 5. Métricas a Recolectar

### 5.1 Planificación

- Tiempo de espera promedio
- Tiempo de retorno promedio
- Tiempo de respuesta promedio
- Utilización de CPU
- Throughput (procesos/unidad de tiempo)
- Número de cambios de contexto

### 5.2 Memoria

- Número de fallos de página
- Tasa de fallos de página
- Número de reemplazos
- Utilización de marcos
- Hit ratio de páginas

### 5.3 Archivos

- Número de accesos exitosos
- Número de conflictos
- Tiempo promedio de espera por archivo
- Distribución de bloqueos (lectura vs escritura)

## 6. Interfaz de Usuario (Terminal)

### 6.1 Menú Principal

```
=== SIMULADOR DE SISTEMA OPERATIVO ===
1. Configurar simulación
2. Ejecutar simulación
3. Ver estadísticas
4. Salir
```

### 6.2 Configuración

- Seleccionar algoritmo de planificación
- Configurar quantum (Round Robin)
- Seleccionar algoritmo de reemplazo
- Definir número de marcos de memoria
- Configurar procesos (manual o automático)

### 6.3 Visualización Durante Ejecución

- Estado de colas de procesos
- Proceso en ejecución
- Uso de memoria (marcos ocupados)
- Eventos de acceso a archivos
- Métricas en tiempo real

## 7. Casos de Prueba Propuestos

### 7.1 Planificación

- **Caso 1**: 5 procesos con tiempos similares (comparar algoritmos)
- **Caso 2**: Mix de procesos cortos y largos (eficiencia SJF)
- **Caso 3**: Procesos con diferentes prioridades (starvation)

### 7.2 Memoria

- **Caso 1**: Procesos con alta localidad de referencia
- **Caso 2**: Patrón de acceso aleatorio (thrashing)
- **Caso 3**: Comparar FIFO vs LRU

### 7.3 Archivos

- **Caso 1**: Múltiples lectores concurrentes
- **Caso 2**: Lectores y escritores mezclados
- **Caso 3**: Alta contención en archivos populares

## 8. Tecnologías y Compilación

### 8.1 Lenguaje y Estándares

- Lenguaje: C (C99 o superior)
- Compilador: GCC o Clang
- Sistema: Multiplataforma (POSIX)

### 8.2 Compilación

```bash
gcc -o simulador main.c proceso.c memoria.c archivo.c planificador.c -std=c99 -Wall -Wextra
```

### 8.3 Ejecución

```bash
./simulador [opciones]
  -a <algoritmo>  : rr (Round Robin), sjf, prioridad
  -q <quantum>    : Quantum para Round Robin
  -r <reemplazo>  : fifo, lru
  -m <marcos>     : Número de marcos de memoria
  -p <num>        : Número de procesos a simular
```

## 9. Cronograma de Desarrollo

- **Fase 1** (Actual): Diseño y documentación - 1 semana
- **Fase 2**: Planificación de procesos - 2 semanas
- **Fase 3**: Gestión de memoria - 2 semanas
- **Fase 4**: Gestión de archivos - 1.5 semanas
- **Fase 5**: Pruebas e informes - 1.5 semanas

**Total estimado**: 8 semanas

## 10. Referencias

- Silberschatz, A., Galvin, P. B., & Gagne, G. (2018). _Operating System Concepts_ (10th ed.)
- Tanenbaum, A. S., & Bos, H. (2014). _Modern Operating Systems_ (4th ed.)
- Documentación POSIX para hilos y sincronización
