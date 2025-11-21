# 🎬 Guía de Presentación en Video - Simulador de Sistema Operativo

**Duración estimada:** 8-12 minutos  
**Objetivo:** Demostrar el funcionamiento completo del simulador y explicar conceptos clave

---

## 📋 Preparación Antes de Grabar

### Checklist Pre-Grabación

- [ ] Compilar el proyecto sin errores
- [ ] Cerrar aplicaciones innecesarias
- [ ] Ajustar tamaño de fuente de la terminal (más grande para video)
- [ ] Preparar ventana de VS Code con archivos clave abiertos
- [ ] Tener esta guía a mano (en segunda pantalla si es posible)
- [ ] Probar audio del micrófono
- [ ] Limpiar escritorio y terminal

### Comandos de Preparación

```powershell
# Limpiar pantalla y compilar
Clear-Host
gcc -o simulador.exe main.c proceso.c memoria.c archivo.c planificador.c -std=c99

# Verificar que compiló correctamente
if (Test-Path simulador.exe) {
    Write-Host "✓ Compilación exitosa" -ForegroundColor Green
}
```

---

## 🎥 GUIÓN COMPLETO DEL VIDEO

---

## ESCENA 1: Introducción (1 minuto)

### 🎬 Toma: Pantalla completa del escritorio

**GUION:**

> "Hola, bienvenidos. Soy [Tu nombre] y en este video les presentaré mi proyecto final de Sistemas Operativos: un simulador completo de SO desarrollado en C.
>
> Este simulador implementa tres componentes fundamentales:
>
> - Primero, la **planificación de procesos** con tres algoritmos diferentes
> - Segundo, la **gestión de memoria virtual** mediante paginación por demanda
> - Y tercero, la **gestión de archivos** con control de acceso concurrente
>
> Vamos a ver cómo funciona."

### 🎬 Acción:

- Mostrar carpeta del proyecto en VS Code
- Scroll rápido por los archivos principales

---

## ESCENA 2: Estructura del Proyecto (1 minuto)

### 🎬 Toma: VS Code con explorador de archivos visible

**GUION:**

> "El proyecto está organizado de forma modular. Tenemos:
>
> - Los archivos de código fuente en C: proceso, memoria, archivo y planificador
> - Cada uno con su header correspondiente
> - El main.c que orquesta todo el sistema
> - Y documentación completa: especificación técnica, informe y pruebas
>
> La compilación es simple usando GCC y el estándar C99."

### 🎬 Acción:

```powershell
# Mostrar árbol de archivos
Get-ChildItem *.c, *.h | Format-Table Name, Length

# Compilar
gcc -o simulador.exe main.c proceso.c memoria.c archivo.c planificador.c -std=c99
```

---

## ESCENA 3: Primera Ejecución - Menú Interactivo (2 minutos)

### 🎬 Toma: Terminal en pantalla completa

**GUION:**

> "Ejecutemos el simulador. Tiene dos modos: interactivo y línea de comandos.
> Primero veamos el modo interactivo."

### 🎬 Acción:

```powershell
.\simulador.exe
```

**GUION mientras aparece el menú:**

> "El menú nos ofrece varias opciones:
>
> - Opción 1: Configurar los parámetros de la simulación
> - Opción 2: Ejecutar con la configuración actual
> - Opción 3: Simulación rápida con valores predeterminados
> - Opción 4: Comparar diferentes algoritmos - esta es muy interesante
> - Y opción 5 para salir
>
> Vamos a configurar primero."

### 🎬 Acción:

```
Seleccionar opción: 1

[En el menú de configuración:]
Algoritmo de planificación: 1 (Round Robin)
Quantum: 4
Algoritmo de reemplazo: 2 (LRU)
Marcos de memoria: 32
Procesos: 8
Archivos: 5
```

**GUION:**

> "He configurado Round Robin con quantum 4, que significa que cada proceso
> ejecutará como máximo 4 unidades de tiempo antes de ceder la CPU.
> Usaremos LRU para reemplazo de páginas, que es más eficiente que FIFO.
> Y simularemos 8 procesos compitiendo por 32 marcos de memoria."

---

## ESCENA 4: Ejecución de Simulación - Round Robin (2.5 minutos)

### 🎬 Toma: Terminal mostrando la ejecución

**GUION:**

> "Ahora ejecutemos la simulación."

### 🎬 Acción:

```
Seleccionar opción: 2
[Presionar Enter para iniciar]
```

**GUION mientras se ejecuta:**

> "Observen la salida. El simulador muestra en tiempo real:
>
> [Cuando aparezca el primer reporte]
>
> - El tiempo actual de la simulación
> - Cuántos procesos han completado
> - Qué proceso está ejecutando actualmente y cuánto le falta
> - La cola de procesos listos esperando su turno
> - La cola de procesos bloqueados, esperando acceso a archivos
> - El estado de la memoria: marcos ocupados
> - Y el estado de los archivos: cuáles están bloqueados
>
> Esto va actualizándose cada 20 unidades de tiempo.
>
> [Esperar a que termine]
>
> Perfecto, la simulación terminó. Veamos las métricas."

**GUION al ver métricas finales:**

> "Las métricas de planificación nos muestran:
>
> - Tiempo de espera promedio: cuánto esperaron los procesos en cola
> - Tiempo de retorno: desde que llegaron hasta que terminaron
> - Tiempo de respuesta: cuánto tardaron en ejecutar por primera vez
> - Utilización de CPU: en este caso más del 90%, excelente
> - Y los cambios de contexto: cuántas veces cambió de proceso
>
> Las estadísticas de memoria muestran:
>
> - Total de accesos a páginas
> - Fallos de página: cuando la página no estaba en memoria
> - La tasa de aciertos o hit ratio
> - Y el número de reemplazos necesarios
>
> Y para archivos vemos:
>
> - Accesos exitosos de lectura y escritura
> - Conflictos cuando dos procesos querían el mismo archivo
> - Y cómo se resolvieron esos conflictos"

---

## ESCENA 5: Comparación de Algoritmos (3 minutos)

### 🎬 Toma: Terminal limpia

**GUION:**

> "Ahora viene lo más interesante: comparar los tres algoritmos de planificación.
> El simulador ejecutará tres veces con los MISMOS procesos pero diferente algoritmo.
> Esto nos permite ver objetivamente cuál funciona mejor."

### 🎬 Acción:

```powershell
# Si saliste del programa, volver a ejecutar
.\simulador.exe
```

```
Seleccionar opción: 4
[Presionar Enter para iniciar comparación]
```

**GUION durante la ejecución:**

> "Primero ejecuta Round Robin...
> [esperar]
> Ahora Shortest Job First o SJF...
> [esperar]
> Y finalmente por Prioridad...
> [esperar]
>
> Excelente, ahora tenemos el resumen comparativo."

**GUION en el resumen:**

> "Miren esta tabla comparativa:
>
> Round Robin tiene buen tiempo de respuesta porque atiende rápido a todos,
> pero genera muchos cambios de contexto por el quantum pequeño.
>
> Shortest Job First minimiza el tiempo de espera, como predice la teoría.
> Es el más eficiente en tiempo pero puede hacer esperar mucho a procesos largos.
>
> Prioridad es útil cuando necesitamos diferenciar procesos críticos,
> aunque en promedio no es el más eficiente.
>
> Cada algoritmo tiene su lugar según los requisitos del sistema."

---

## ESCENA 6: Modo Línea de Comandos (1.5 minutos)

### 🎬 Toma: Terminal limpia

**GUION:**

> "También podemos ejecutar el simulador por línea de comandos,
> útil para automatización o pruebas específicas.
> Veamos algunos ejemplos."

### 🎬 Acción:

```powershell
Clear-Host

# Ejemplo 1: SJF con configuración específica
Write-Host "`n=== Ejemplo 1: SJF con 10 procesos ===" -ForegroundColor Yellow
.\simulador.exe -a sjf -r lru -m 64 -p 10 -f 5
```

**GUION:**

> "Este comando ejecuta SJF con 64 marcos de memoria y 10 procesos.
> [Mientras se ejecuta]
> Vemos que completa más rápido porque no hay interacción."

### 🎬 Acción:

```powershell
# Ejemplo 2: Comparar FIFO vs LRU con memoria limitada
Write-Host "`n=== Ejemplo 2: Memoria restringida con FIFO ===" -ForegroundColor Yellow
.\simulador.exe -a rr -q 4 -r fifo -m 16 -p 8 -f 3
```

**GUION:**

> "Ahora probemos con memoria MUY limitada: solo 16 marcos para 8 procesos.
> Esto causa muchos fallos de página, conocido como thrashing.
> [Esperar estadísticas de memoria]
> Vean la tasa de fallos: más del 70%. La memoria es insuficiente."

---

## ESCENA 7: Código Destacado (1 minuto)

### 🎬 Toma: VS Code con código

**GUION:**

> "Rápidamente, veamos algunas partes clave del código."

### 🎬 Acción: Abrir archivos y mostrar:

**1. proceso.h (líneas 8-32):**

```c
// Estructura PCB
typedef struct Proceso {
    int pid;
    EstadoProceso estado;
    int prioridad;
    int tiempo_rafaga;
    int tiempo_restante;
    // ... memoria y archivos
} Proceso;
```

**GUION:**

> "La estructura Proceso o PCB contiene toda la información de un proceso:
> identificador, estado, tiempos, páginas de memoria y archivos que usa."

**2. planificador.c - función seleccionar_siguiente_proceso:**

**GUION:**

> "El planificador selecciona el siguiente proceso según el algoritmo:
> Round Robin usa FIFO simple, SJF busca el de menor tiempo restante,
> y Prioridad busca el de mayor prioridad."

**3. memoria.c - función acceder_pagina:**

**GUION:**

> "Cuando un proceso accede a memoria, primero verificamos si la página está cargada.
> Si no, es un fallo de página y debemos cargarla, posiblemente reemplazando otra."

---

## ESCENA 8: Documentación (30 segundos)

### 🎬 Toma: Explorador de archivos / VS Code

**GUION:**

> "El proyecto incluye documentación completa:
>
> - Especificación técnica con el diseño de todas las estructuras
> - Informe técnico con análisis de resultados y decisiones
> - Y documento de pruebas con 10 casos de prueba validados
> - Más de 150 páginas de documentación en total."

### 🎬 Acción:

```powershell
Get-ChildItem *.md | Format-Table Name, Length
```

---

## ESCENA 9: Cierre y Conclusiones (1 minuto)

### 🎬 Toma: Pantalla completa o cámara

**GUION:**

> "Para concluir, este simulador demuestra los conceptos fundamentales
> de sistemas operativos de forma práctica y educativa.
>
> Hemos implementado:
>
> - Tres algoritmos de planificación con métricas detalladas
> - Gestión de memoria virtual con paginación y dos algoritmos de reemplazo
> - Control de acceso concurrente a archivos
> - Todo desarrollado en C, sin memory leaks, completamente funcional
>
> El código es modular, está bien documentado y puede servir como
> base para entender cómo funcionan estos componentes en un SO real.
>
> Los resultados muestran claramente los trade-offs entre algoritmos:
> SJF minimiza espera pero puede causar inanición,
> Round Robin es equitativo pero genera overhead,
> y cada uno tiene su propósito según el contexto.
>
> Gracias por ver este video. El código completo y la documentación
> están disponibles en el repositorio del proyecto."

---

## 📊 ESCENAS OPCIONALES (Si tienes tiempo extra)

### ESCENA EXTRA 1: Prueba de Estrés

```powershell
Write-Host "`n=== Prueba con 50 procesos ===" -ForegroundColor Red
.\simulador.exe -a rr -q 4 -r lru -m 128 -p 50 -f 20
```

**GUION:**

> "Probemos el límite: 50 procesos simultáneos con 128 marcos.
> El simulador maneja la carga sin problemas y completa todos los procesos."

### ESCENA EXTRA 2: Mostrar Makefile

```powershell
Get-Content Makefile | Select-Object -First 30
```

**GUION:**

> "Incluye un Makefile para compilación automatizada con varios targets útiles."

---

## 🎯 TIPS PARA LA GRABACIÓN

### Durante la Grabación:

1. **Habla con claridad y pausas**

   - No te apresures
   - Haz pausas después de ejecutar comandos
   - Da tiempo a que se vean los resultados

2. **Señala con el cursor**

   - Mueve el mouse a las partes importantes
   - Resalta números clave en las métricas

3. **Gestiona el tiempo**

   - Si una simulación tarda mucho, está bien hacer un corte de edición
   - Puedes acelerar las partes repetitivas en edición

4. **Si te equivocas**
   - No te preocupes, puedes pausar y reiniciar esa sección
   - O déjalo y edita después

### Ajustes de Terminal Recomendados:

```powershell
# Antes de grabar, aumentar tamaño de fuente
# En PowerShell: Ctrl + rueda del mouse
# O: Click derecho > Properties > Font > Size: 16-18

# Color scheme claro si es posible (más visible en video)
```

### Estructura de Carpeta para Grabación:

```
Tener abierto:
1. Terminal PowerShell (principal)
2. VS Code con proyecto (secundario)
3. Esta guía (segunda pantalla o impresa)
```

---

## ⏱️ CRONOGRAMA SUGERIDO

| Tiempo        | Escena                    | Duración  |
| ------------- | ------------------------- | --------- |
| 0:00 - 1:00   | Introducción              | 1:00      |
| 1:00 - 2:00   | Estructura del proyecto   | 1:00      |
| 2:00 - 4:00   | Menú y configuración      | 2:00      |
| 4:00 - 6:30   | Ejecución Round Robin     | 2:30      |
| 6:30 - 9:30   | Comparación de algoritmos | 3:00      |
| 9:30 - 11:00  | Línea de comandos         | 1:30      |
| 11:00 - 12:00 | Código y documentación    | 1:00      |
| 12:00 - 13:00 | Cierre                    | 1:00      |
| **TOTAL**     |                           | **13:00** |

Puedes ajustar según tu tiempo disponible.

---

## 📝 CHECKLIST POST-GRABACIÓN

Antes de entregar el video, verifica:

- [ ] Audio claro y sin ruido de fondo
- [ ] Texto en terminal legible (fuente grande)
- [ ] Todas las ejecuciones completadas exitosamente
- [ ] Métricas visibles y explicadas
- [ ] Transiciones suaves entre escenas
- [ ] Duración total 8-15 minutos
- [ ] Introducción y cierre presentes
- [ ] Créditos al final (opcional)

---

## 🎬 GUION ALTERNATIVO CORTO (5-7 minutos)

Si necesitas una versión más breve:

1. **Intro (30 seg)** - Qué es el proyecto
2. **Compilación (30 seg)** - Mostrar código y compilar
3. **Ejecución rápida (2 min)** - Opción 3 del menú
4. **Comparación (2.5 min)** - Opción 4 del menú
5. **Línea comandos (1 min)** - Un ejemplo rápido
6. **Cierre (30 seg)** - Conclusiones

---

## 📧 PLANTILLA DE DESCRIPCIÓN PARA EL VIDEO

```
Simulador de Sistema Operativo - Proyecto Final

Simulador educativo de SO desarrollado en C que implementa:
✓ Planificación de procesos (Round Robin, SJF, Prioridad)
✓ Gestión de memoria virtual (Paginación, FIFO, LRU)
✓ Gestión de archivos (Control de concurrencia)

Características:
- 2,500+ líneas de código en C
- Sin memory leaks (verificado con Valgrind)
- Documentación completa (150+ páginas)
- 10 casos de prueba validados

Tecnologías: C99, GCC, Estructuras de datos, Algoritmos de SO

Repositorio: [Tu enlace de GitHub]
Curso: Sistemas Operativos - UPTC
```

---

## 🎥 ¡LISTO PARA GRABAR!

**Último recordatorio antes de empezar:**

1. Respira profundo
2. Sonríe (se nota en la voz)
3. Disfruta mostrando tu trabajo
4. Tú conoces el proyecto mejor que nadie

**¡Éxito con tu grabación! 🚀**
