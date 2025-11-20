#ifndef ARCHIVO_H
#define ARCHIVO_H

#define MAX_ARCHIVOS 20
#define MAX_ESPERA 10

// Modos de acceso a archivos
typedef enum
{
    LECTURA = 0,
    ESCRITURA = 1
} ModoAcceso;

// Estado de bloqueo de un archivo
typedef struct
{
    int pid;         // Proceso que tiene el bloqueo (-1 si libre)
    ModoAcceso modo; // Tipo de bloqueo
    int activo;      // 1 = bloqueado, 0 = libre
} BloqueoArchivo;

// Estructura de archivo simulado
typedef struct
{
    char nombre[64];        // Nombre del archivo
    int tamanio;            // Tamaño en bytes
    BloqueoArchivo bloqueo; // Estado de bloqueo principal
    int lectores_activos;   // Contador de lectores simultáneos
    int escritor_activo;    // 1 si hay escritor, 0 si no

    // Cola de procesos esperando acceso
    int cola_espera[MAX_ESPERA]; // Array de PIDs
    int num_esperando;           // Cantidad en espera
} Archivo;

// Gestor de archivos del sistema
typedef struct
{
    Archivo archivos[MAX_ARCHIVOS];
    int num_archivos;

    // Estadísticas
    int num_conflictos; // Contador de conflictos
    int num_accesos_exitosos;
    int num_accesos_lectura;
    int num_accesos_escritura;
    int num_bloqueos_resueltos;
} GestorArchivos;

// Funciones de gestión de archivos
GestorArchivos *crear_gestor_archivos(int num_archivos);
void destruir_gestor_archivos(GestorArchivos *ga);
int solicitar_acceso_archivo(GestorArchivos *ga, const char *nombre, int pid, ModoAcceso modo);
int liberar_acceso_archivo(GestorArchivos *ga, const char *nombre, int pid);
int verificar_conflicto(Archivo *archivo, ModoAcceso modo);
void procesar_cola_espera(GestorArchivos *ga, int indice_archivo);
void imprimir_estado_archivos(GestorArchivos *ga);
void imprimir_estadisticas_archivos(GestorArchivos *ga);
int buscar_archivo(GestorArchivos *ga, const char *nombre);

#endif
