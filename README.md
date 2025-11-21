# Simulador de Sistema Operativo

Simulador educativo de sistema operativo que implementa planificación de procesos, gestión de memoria virtual y gestión de archivos con acceso concurrente.

## Características

- **Planificación de Procesos**: Round Robin, Shortest Job First (SJF) y por Prioridad
- **Gestión de Memoria**: Paginación por demanda con algoritmos FIFO y LRU
- **Gestión de Archivos**: Control de acceso concurrente con bloqueos lectores-escritores
- **Métricas Detalladas**: Tiempo de espera, retorno, respuesta, utilización de CPU, fallos de página
- **Interfaz por Terminal**: Menú interactivo y modo de línea de comandos

## Compilación

### Requisitos

- Compilador GCC (MinGW en Windows)
- Make (opcional pero recomendado)

### Compilar con Make

```bash
make
```

### Compilar manualmente

```bash
gcc -o simulador main.c proceso.c memoria.c archivo.c planificador.c -std=c99 -Wall -Wextra
```

### Windows (PowerShell)

```powershell
gcc -o simulador.exe main.c proceso.c memoria.c archivo.c planificador.c -std=c99 -Wall -Wextra
```

## Uso

### Modo Interactivo

```bash
./simulador
```

Muestra un menú con opciones para:

1. Configurar parámetros de simulación
2. Ejecutar simulación con configuración actual
3. Ejecutar simulación rápida con valores por defecto
4. Comparar diferentes algoritmos de planificación
5. Salir

### Modo Línea de Comandos

```bash
./simulador -a <algoritmo> -q <quantum> -r <reemplazo> -m <marcos> -p <procesos> -f <archivos>
```

**Parámetros:**

- `-a <algoritmo>`: `rr` (Round Robin), `sjf` (Shortest Job First), `prioridad`
- `-q <quantum>`: Quantum para Round Robin (1-10, default: 4)
- `-r <reemplazo>`: `fifo` o `lru` (default: lru)
- `-m <marcos>`: Número de marcos de memoria (16-128, default: 32)
- `-p <procesos>`: Número de procesos a simular (5-50, default: 10)
- `-f <archivos>`: Número de archivos del sistema (3-20, default: 5)

**Ejemplos:**

```bash
# Round Robin con quantum 5, LRU, 64 marcos, 15 procesos
./simulador -a rr -q 5 -r lru -m 64 -p 15 -f 8

# SJF con FIFO, configuración pequeña
./simulador -a sjf -r fifo -m 16 -p 8 -f 3

# Prioridad con configuración grande
./simulador -a prioridad -r lru -m 128 -p 30 -f 12
```

## 🧪 Pruebas Rápidas

### Simulación rápida por defecto

```bash
make quick
```

### Comparar algoritmos de planificación

```bash
make compare
```

### Comparar FIFO vs LRU

```bash
make test-memory
```

## Métricas Reportadas

### Planificación

- **Tiempo de espera promedio**: Tiempo que los procesos pasan esperando en cola
- **Tiempo de retorno promedio**: Tiempo desde llegada hasta finalización
- **Tiempo de respuesta promedio**: Tiempo hasta primera ejecución
- **Utilización de CPU**: Porcentaje de tiempo que la CPU estuvo ocupada
- **Throughput**: Procesos completados por unidad de tiempo
- **Cambios de contexto**: Número de veces que se cambió de proceso

### Memoria

- **Total de accesos**: Accesos a páginas realizados
- **Fallos de página**: Páginas no encontradas en memoria
- **Número de reemplazos**: Veces que se reemplazó una página
- **Tasa de fallos**: Porcentaje de accesos que fueron fallos
- **Hit ratio**: Porcentaje de accesos exitosos
- **Marcos ocupados**: Utilización de memoria física

### Archivos

- **Accesos exitosos**: Accesos concedidos sin conflicto
  - Lectura
  - Escritura
- **Conflictos detectados**: Intentos de acceso que requirieron espera
- **Bloqueos resueltos**: Procesos desbloqueados
- **Tasa de conflicto**: Porcentaje de accesos con conflicto

## Estructura del Proyecto

```
proyecto-final-so/
├── main.c              # Programa principal y menú
├── proceso.h/c         # Gestión de procesos (PCB)
├── memoria.h/c         # Gestión de memoria (paginación)
├── archivo.h/c         # Gestión de archivos (bloqueos)
├── planificador.h/c    # Planificador y algoritmos
├── Makefile            # Compilación automatizada
├── README.md           # Este archivo
├── ESPECIFICACION.md   # Documentación técnica completa
└── INFORME_TECNICO.md  # Informe del proyecto
```

## Algoritmos Implementados

### Planificación de Procesos

1. **Round Robin**

   - Cola circular FIFO
   - Quantum configurable
   - Apropiativo

2. **Shortest Job First (SJF)**

   - Selecciona proceso con menor tiempo restante
   - No apropiativo
   - Minimiza tiempo de espera promedio

3. **Prioridad**
   - Selecciona proceso con mayor prioridad (menor número)
   - Puede causar inanición
   - No apropiativo

### Gestión de Memoria

1. **FIFO (First In, First Out)**

   - Reemplaza página más antigua
   - Simple de implementar
   - Puede sufrir Anomalía de Belady

2. **LRU (Least Recently Used)**
   - Reemplaza página menos usada recientemente
   - Mejor rendimiento generalmente
   - Requiere tracking de accesos

### Gestión de Archivos

- **Problema Lectores-Escritores**
  - Múltiples lectores simultáneos permitidos
  - Escritor requiere acceso exclusivo
  - Cola de espera para procesos bloqueados
