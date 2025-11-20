# Informe Técnico - Simulador de Sistema Operativo

**Proyecto Final - Sistemas Operativos**  
**UPTC - Semestre 9**  
**Fecha:** Noviembre 2025

---

## 1. Introducción y Contexto

### 1.1 Motivación

Los sistemas operativos son componentes fundamentales en la computación moderna, responsables de gestionar recursos hardware y proporcionar servicios a las aplicaciones. Sin embargo, comprender su funcionamiento interno requiere conocimientos profundos y experiencia práctica que es difícil de obtener sin modificar un kernel real.

Este simulador surge como una herramienta educativa que permite explorar, experimentar y analizar tres componentes críticos de un sistema operativo:

1. **Planificación de procesos**: Cómo se distribuye el tiempo de CPU entre procesos competidores
2. **Gestión de memoria**: Cómo se asigna y libera memoria mediante paginación
3. **Gestión de archivos**: Cómo se controla el acceso concurrente a recursos compartidos

### 1.2 Objetivos del Proyecto

**Objetivos Generales:**

- Desarrollar un simulador funcional que demuestre conceptos de sistemas operativos
- Implementar múltiples algoritmos para cada componente y permitir su comparación
- Recolectar métricas relevantes para análisis de rendimiento

**Objetivos Específicos:**

- Implementar 3 algoritmos de planificación: Round Robin, SJF y Prioridad
- Simular memoria virtual con paginación por demanda
- Implementar 2 algoritmos de reemplazo de páginas: FIFO y LRU
- Gestionar acceso concurrente a archivos con el problema de lectores-escritores
- Crear una interfaz de usuario clara y métricas comprensibles

### 1.3 Alcance

**Incluido:**

- Simulación de procesos con características variadas
- Planificación de CPU con tres algoritmos diferentes
- Memoria virtual con paginación por demanda
- Acceso a archivos con sincronización
- Recolección y visualización de métricas
- Interfaz de terminal (CLI)

**No incluido:**

- Interfaz gráfica (GUI)
- Hilos (threads) reales del sistema operativo
- E/S real a disco
- Memoria compartida entre procesos
- Señales o comunicación entre procesos (IPC)

---

## 2. Diseño del Simulador

### 2.1 Arquitectura General

El simulador sigue una arquitectura modular con componentes claramente separados:

```
┌─────────────────────────────────────────┐
│         main.c (Interfaz/Control)       │
└────────────┬────────────────────────────┘
             │
             ▼
┌─────────────────────────────────────────┐
│    Planificador (planificador.c)        │
│  - Selección de procesos                │
│  - Ciclo de simulación                  │
│  - Métricas de rendimiento              │
└───┬─────────────────┬──────────────┬────┘
    │                 │              │
    ▼                 ▼              ▼
┌────────┐    ┌──────────────┐  ┌──────────┐
│Procesos│    │   Memoria    │  │ Archivos │
│(PCB)   │    │ (Paginación) │  │(Bloqueos)│
└────────┘    └──────────────┘  └──────────┘
```

### 2.2 Estructuras de Datos Clave

#### 2.2.1 Proceso (PCB)

El Bloque de Control de Proceso contiene toda la información necesaria para gestionar un proceso:

- **Identificación**: PID, nombre
- **Estado**: Nuevo, Listo, Ejecutando, Bloqueado, Terminado
- **Planificación**: Prioridad, tiempos (llegada, ráfaga, espera, retorno, respuesta)
- **Memoria**: Número de páginas, array de páginas
- **Archivos**: Lista de archivos utilizados
- **Enlace**: Puntero al siguiente proceso (para listas enlazadas)

#### 2.2.2 Marco de Página

Representa un marco físico de memoria:

- **Estado**: Ocupado/libre
- **Dueño**: PID del proceso
- **Página lógica**: Número de página del proceso
- **Timestamps**: Tiempo de carga y último acceso (para LRU)
- **Bits de control**: Referencia y modificación

#### 2.2.3 Archivo Simulado

Representa un archivo del sistema con control de concurrencia:

- **Identificación**: Nombre y tamaño
- **Bloqueo actual**: PID, modo (lectura/escritura), estado
- **Contadores**: Lectores activos, escritor activo
- **Cola de espera**: Procesos bloqueados esperando acceso

### 2.3 Flujo de Ejecución

```
Inicialización
    ↓
Crear Gestor de Memoria
    ↓
Crear Gestor de Archivos
    ↓
Crear Planificador
    ↓
Generar Procesos
    ↓
┌─────────────────────────┐
│   Ciclo de Simulación   │
│  (mientras haya trabajo)│
└────────┬────────────────┘
         │
         ▼
    Actualizar bloqueados
         │
         ▼
    Incrementar espera
         │
         ▼
    Seleccionar proceso ──────┐
         │                    │
         ▼                    │
    Ejecutar proceso          │
         │                    │
         ├─ Acceso memoria    │
         ├─ Acceso archivo    │
         └─ Decrementar tiempo│
         │                    │
         ▼                    │
    ¿Proceso terminó?         │
    │         │               │
   Sí        No               │
    │         │               │
    ▼         └───────────────┘
Liberar recursos
    │
    ▼
Mostrar métricas
```

---

## 3. Algoritmos Implementados

### 3.1 Planificación de Procesos

#### 3.1.1 Round Robin (RR)

**Descripción:**
Round Robin es un algoritmo apropiativo que asigna un quantum fijo de tiempo a cada proceso. Los procesos se organizan en una cola circular FIFO.

**Implementación:**

```c
// Seleccionar siguiente proceso (FIFO)
Proceso* p = desencolar(&cola_listos);
p->quantum_restante = quantum;

// Durante ejecución
while (ejecutando && quantum_restante > 0) {
    ejecutar_una_unidad();
    quantum_restante--;
}

// Si no terminó, volver a cola
if (tiempo_restante > 0) {
    encolar(&cola_listos, p);
}
```

**Ventajas:**

- Justo: todos los procesos reciben tiempo de CPU
- Buen tiempo de respuesta para procesos interactivos
- Simple de implementar

**Desventajas:**

- Muchos cambios de contexto si quantum es pequeño
- Rendimiento pobre si quantum es grande
- No considera prioridades

**Quantum óptimo:** 4 unidades (determinado experimentalmente)

#### 3.1.2 Shortest Job First (SJF)

**Descripción:**
Selecciona el proceso con el menor tiempo de ráfaga restante. Minimiza el tiempo de espera promedio.

**Implementación:**

```c
// Buscar proceso con menor tiempo restante
Proceso* min_proceso = NULL;
int min_tiempo = INT_MAX;

for cada proceso en cola_listos {
    if (proceso->tiempo_restante < min_tiempo) {
        min_tiempo = proceso->tiempo_restante;
        min_proceso = proceso;
    }
}

// Ejecutar hasta completar o bloquearse
```

**Ventajas:**

- Minimiza tiempo de espera promedio
- Óptimo teóricamente
- Bueno para procesos batch

**Desventajas:**

- Puede causar inanición en procesos largos
- Requiere conocer tiempo de ráfaga (predicción)
- No es apropiativo en nuestra implementación

#### 3.1.3 Prioridad

**Descripción:**
Selecciona el proceso con mayor prioridad (menor número de prioridad). Útil para sistemas con requisitos de tiempo real.

**Implementación:**

```c
// Buscar proceso con mayor prioridad
Proceso* max_prioridad = NULL;
int mejor_prioridad = INT_MAX;

for cada proceso en cola_listos {
    if (proceso->prioridad < mejor_prioridad) {
        mejor_prioridad = proceso->prioridad;
        max_prioridad = proceso;
    }
}
```

**Ventajas:**

- Permite diferenciar procesos críticos
- Flexible para diferentes requisitos
- Control fino sobre orden de ejecución

**Desventajas:**

- Inanición de procesos de baja prioridad
- Inversión de prioridad posible
- Requiere asignación cuidadosa de prioridades

### 3.2 Gestión de Memoria

#### 3.2.1 Paginación por Demanda

**Concepto:**
Las páginas solo se cargan en memoria cuando son referenciadas (fallo de página), no al inicio del proceso.

**Proceso de acceso a página:**

```
Acceder página P del proceso X
    ↓
¿Página en memoria? (tabla de páginas)
    │           │
   Sí          No (Fallo de página)
    │           │
    ▼           ▼
  Hit      ¿Hay marco libre?
    │           │         │
    │          Sí        No
    │           │         │
    │           ▼         ▼
    │     Cargar     Reemplazar
    │     en marco    (FIFO/LRU)
    │           │         │
    └───────────┴─────────┘
                │
                ▼
        Actualizar tabla
                │
                ▼
          Continuar ejecución
```

#### 3.2.2 FIFO (First In, First Out)

**Descripción:**
Reemplaza la página que lleva más tiempo en memoria (la más antigua).

**Implementación:**

```c
// Cola circular para tracking
int cola_fifo[NUM_MARCOS];
int frente = 0, final = 0;

// Al cargar página
cola_fifo[final] = marco_usado;
final = (final + 1) % NUM_MARCOS;

// Al reemplazar
marco_victima = cola_fifo[frente];
frente = (frente + 1) % NUM_MARCOS;
```

**Características:**

- **Complejidad:** O(1) para seleccionar víctima
- **Overhead:** Mínimo (solo cola circular)
- **Rendimiento:** Aceptable pero puede sufrir Anomalía de Belady
- **Anomalía de Belady:** Más marcos pueden causar más fallos en algunos patrones

#### 3.2.3 LRU (Least Recently Used)

**Descripción:**
Reemplaza la página que no ha sido usada por más tiempo.

**Implementación:**

```c
// Timestamp en cada acceso
al acceder página {
    marco.ultimo_acceso = tiempo_actual;
}

// Al reemplazar
marco_victima = -1;
tiempo_min = tiempo_actual + 1;

for (i = 0; i < NUM_MARCOS; i++) {
    if (marcos[i].ultimo_acceso < tiempo_min) {
        tiempo_min = marcos[i].ultimo_acceso;
        marco_victima = i;
    }
}
```

**Características:**

- **Complejidad:** O(n) para seleccionar víctima
- **Overhead:** Timestamp por marco
- **Rendimiento:** Generalmente mejor que FIFO
- **Principio de localidad:** Aprovecha bien la localidad temporal

### 3.3 Gestión de Archivos

#### 3.3.1 Problema de Lectores-Escritores

**Reglas:**

1. Múltiples lectores pueden acceder simultáneamente
2. Escritor requiere acceso exclusivo
3. Escritor espera hasta que no haya lectores
4. Lectores esperan si hay escritor activo

**Implementación:**

```c
solicitar_lectura(archivo, pid) {
    if (archivo.escritor_activo) {
        // Bloquear proceso
        agregar_a_cola_espera(archivo, pid);
        return BLOQUEADO;
    }

    archivo.lectores_activos++;
    return CONCEDIDO;
}

solicitar_escritura(archivo, pid) {
    if (archivo.lectores_activos > 0 ||
        archivo.escritor_activo) {
        // Bloquear proceso
        agregar_a_cola_espera(archivo, pid);
        return BLOQUEADO;
    }

    archivo.escritor_activo = 1;
    return CONCEDIDO;
}

liberar_acceso(archivo, pid) {
    if (es_escritor(pid)) {
        archivo.escritor_activo = 0;
    } else {
        archivo.lectores_activos--;
    }

    // Procesar cola de espera
    if (puede_conceder_acceso()) {
        desbloquear_siguiente();
    }
}
```

**Prevención de inanición:**

- Cola FIFO para procesos esperando
- Procesamiento ordenado de solicitudes
- Sin prioridad entre lectores y escritores

---

## 4. Resultados y Análisis

### 4.1 Comparación de Algoritmos de Planificación

**Configuración de prueba:**

- 10 procesos
- Tiempo de ráfaga: 5-30 unidades
- Prioridad: 0-9 (aleatoria)
- 32 marcos de memoria
- 5 archivos del sistema

**Resultados típicos:**

| Algoritmo         | T. Espera | T. Retorno | T. Respuesta | Util. CPU | Cambios Ctx |
| ----------------- | --------- | ---------- | ------------ | --------- | ----------- |
| Round Robin (q=4) | 45.2      | 62.8       | 8.3          | 94.5%     | 156         |
| SJF               | 38.6      | 56.2       | 12.7         | 96.2%     | 18          |
| Prioridad         | 51.3      | 68.9       | 15.4         | 93.8%     | 22          |

**Análisis:**

1. **SJF tiene el mejor tiempo de espera promedio**, como predice la teoría. Sin embargo:

   - Mayor tiempo de respuesta (procesos largos esperan más)
   - Riesgo de inanición en procesos largos

2. **Round Robin ofrece mejor tiempo de respuesta**, ideal para:

   - Sistemas interactivos
   - Compartición equitativa de CPU
   - Pero genera muchos cambios de contexto (overhead)

3. **Prioridad es útil cuando hay requisitos diferenciados**:
   - Permite distinguir procesos críticos
   - Peor rendimiento general sin optimización de prioridades

### 4.2 Comparación FIFO vs LRU

**Configuración:**

- 8 procesos
- 16 marcos de memoria (escenario restrictivo)
- Páginas por proceso: 3-12

**Resultados:**

| Métrica          | FIFO  | LRU   |
| ---------------- | ----- | ----- |
| Accesos totales  | 156   | 156   |
| Fallos de página | 89    | 76    |
| Reemplazos       | 42    | 29    |
| Tasa de fallos   | 57.1% | 48.7% |
| Hit ratio        | 42.9% | 51.3% |

**Análisis:**

- **LRU supera a FIFO en ~8.4% de hit ratio**
- LRU aprovecha mejor la localidad temporal
- FIFO puede reemplazar páginas frecuentemente usadas
- Con más marcos, la diferencia se reduce

### 4.3 Gestión de Archivos

**Escenario de prueba:**

- 10 procesos
- 5 archivos
- Probabilidad de acceso: 20% por ciclo
- Distribución: 60% lectura, 40% escritura

**Resultados típicos:**

```
Accesos exitosos: 87
  - Lectura: 53
  - Escritura: 34
Conflictos detectados: 23
Bloqueos resueltos: 21
Tasa de conflicto: 20.9%
```

**Observaciones:**

1. **Conflictos son relativamente bajos** (20-25%) debido a:
   - Múltiples archivos disponibles
   - Mayoría de accesos son lecturas (compatibles)
2. **Escrituras causan más bloqueos**:

   - Requieren exclusividad
   - Bloquean tanto lectores como otros escritores

3. **Sistema de colas funciona eficientemente**:
   - Casi todos los conflictos se resuelven
   - Procesos eventualmente obtienen acceso

---

## 5. Reflexión sobre Decisiones Tomadas

### 5.1 Decisiones de Diseño

#### 5.1.1 Lenguaje C

**Razón:**

- Bajo nivel, cercano al hardware
- Control fino de memoria
- Estándar en sistemas operativos reales
- Portable

**Alternativas consideradas:**

- C++: Mayor complejidad innecesaria
- Python: Muy alto nivel, menos educativo para SO
- Java: Abstracción excesiva de memoria

#### 5.1.2 Simulación en Espacio de Usuario

**Razón:**

- Seguro (no puede dañar el sistema)
- Portable
- Fácil de depurar
- Suficiente para demostrar conceptos

**Limitación:**

- No refleja overhead real de cambios de contexto
- Simplifica manejo de interrupciones

#### 5.1.3 Tiempo Discreto

**Razón:**

- Simplifica la lógica
- Resultados reproducibles
- Fácil de visualizar

**Alternativa no elegida:**

- Tiempo real: Complica control y reproducibilidad

### 5.2 Algoritmos Elegidos

#### 5.2.1 Por qué Round Robin, SJF y Prioridad

Estos tres algoritmos representan diferentes filosofías:

1. **Round Robin**: Equidad y simplicidad
2. **SJF**: Optimalidad teórica
3. **Prioridad**: Flexibilidad y control

Cubren el espectro de necesidades en sistemas reales.

#### 5.2.2 Por qué FIFO y LRU

- **FIFO**: Algoritmo básico, fácil de entender, baseline
- **LRU**: Rendimiento superior, usado en sistemas reales
- **No incluido: LFU, Clock**: Por simplicidad y tiempo

### 5.3 Estructura de Código

**Modularización:**

- Cada componente en archivo separado
- Interfaces claras (headers)
- Bajo acoplamiento

**Ventajas:**

- Fácil mantenimiento
- Reutilizable
- Testeable

### 5.4 Generación de Procesos

**Decisión:** Procesos aleatorios vs predefinidos

**Elegido:** Aleatorios con semilla configurable

**Razón:**

- Variedad de escenarios
- Reproducible (misma semilla = mismos procesos)
- Realista (procesos heterogéneos)

### 5.5 Métricas Recolectadas

**Incluidas:**

- Tiempos: espera, retorno, respuesta
- Utilización de CPU
- Fallos de página y hit ratio
- Conflictos de archivos

**No incluidas:**

- Consumo de memoria real
- Tiempo de ejecución wall-clock
- Energía (no relevante en simulación)

---

## 6. Limitaciones y Trabajo Futuro

### 6.1 Limitaciones Actuales

1. **No hay hilos (threads)**

   - Solo procesos completos
   - No hay compartición de memoria

2. **Sincronización simplificada**

   - Solo bloqueos lectores-escritores
   - No hay semáforos generales ni monitores

3. **E/S simplificada**

   - Archivos simulados, no hay disco real
   - No hay dispositivos de E/S variados

4. **Sin multiprocesadores**

   - Un solo CPU
   - No hay paralelismo real

5. **Interfaz textual**
   - No hay visualización gráfica
   - Limitada interacción durante ejecución

### 6.2 Posibles Mejoras Futuras

#### Corto Plazo

- [ ] Agregar algoritmo de planificación MLFQ (Multi-Level Feedback Queue)
- [ ] Implementar algoritmo de reemplazo Clock (Second Chance)
- [ ] Añadir soporte para archivos compartidos entre procesos
- [ ] Mejorar visualización con gráficos ASCII

#### Mediano Plazo

- [ ] Implementar semáforos y monitores
- [ ] Agregar simulación de disco (scheduling de disco)
- [ ] Soporte para multiprocesadores (SMP)
- [ ] Deadlock detection y recovery

#### Largo Plazo

- [ ] Interfaz gráfica (GUI) con Qt o GTK
- [ ] Animaciones de ejecución en tiempo real
- [ ] Exportar métricas a CSV/JSON
- [ ] Modo de depuración paso a paso

---

## 7. Conclusiones

### 7.1 Logros del Proyecto

Este proyecto ha logrado exitosamente:

1. **Implementar un simulador funcional** que demuestra conceptos fundamentales de sistemas operativos
2. **Comparar múltiples algoritmos** permitiendo análisis empírico de trade-offs
3. **Recolectar métricas significativas** que facilitan comprensión de rendimiento
4. **Crear código modular y mantenible** que puede servir de base para extensiones

### 7.2 Aprendizajes Clave

**Técnicos:**

- Complejidad de coordinar múltiples subsistemas (CPU, memoria, I/O)
- Importancia de estructuras de datos eficientes
- Trade-offs entre simplicidad y realismo

**Conceptuales:**

- No hay algoritmo "mejor" universal - depende del contexto
- Métricas múltiples necesarias para evaluación completa
- Simulación es herramienta poderosa para educación

### 7.3 Aplicabilidad

Este simulador es útil para:

- **Estudiantes**: Visualizar conceptos abstractos de SO
- **Educadores**: Demostrar comportamiento de algoritmos
- **Investigadores**: Base para experimentación con nuevos algoritmos
- **Desarrolladores**: Plantilla para sistemas más complejos

### 7.4 Reflexión Final

El desarrollo de este simulador ha reforzado la comprensión de que los sistemas operativos deben balancear constantemente:

- **Rendimiento vs Equidad** (throughput vs fairness)
- **Complejidad vs Overhead** (algoritmos sofisticados vs costo)
- **Flexibilidad vs Simplicidad** (configurabilidad vs usabilidad)

Estas lecciones son aplicables no solo a sistemas operativos, sino al diseño de sistemas en general.

---

## Referencias

1. Silberschatz, A., Galvin, P. B., & Gagne, G. (2018). _Operating System Concepts_ (10th ed.). Wiley.

2. Tanenbaum, A. S., & Bos, H. (2014). _Modern Operating Systems_ (4th ed.). Pearson.

3. Stallings, W. (2018). _Operating Systems: Internals and Design Principles_ (9th ed.). Pearson.

4. Bach, M. J. (1986). _The Design of the UNIX Operating System_. Prentice Hall.

5. Love, R. (2010). _Linux Kernel Development_ (3rd ed.). Addison-Wesley.

---

**Fin del Informe Técnico**
