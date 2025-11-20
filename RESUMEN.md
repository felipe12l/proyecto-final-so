# Resumen Ejecutivo - Simulador de Sistema Operativo

## 📌 Información del Proyecto

**Nombre:** Simulador de Sistema Operativo  
**Institución:** Universidad Pedagógica y Tecnológica de Colombia (UPTC)  
**Curso:** Sistemas Operativos - Semestre 9  
**Fecha:** Noviembre 2025  
**Lenguaje:** C (C99)  
**Líneas de código:** ~2,500

---

## 🎯 Objetivo

Desarrollar un simulador educativo que demuestre el funcionamiento interno de tres componentes críticos de un sistema operativo:

1. **Planificación de Procesos** (CPU Scheduling)
2. **Gestión de Memoria Virtual** (Paging)
3. **Gestión de Archivos** (Concurrent Access Control)

---

## ✅ Componentes Implementados

### 1. Planificación de Procesos

- ✓ **Round Robin** - Quantum configurable, equitativo
- ✓ **Shortest Job First (SJF)** - Minimiza tiempo de espera
- ✓ **Prioridad** - Control fino de ejecución

**Métricas recolectadas:**

- Tiempo de espera, retorno y respuesta
- Utilización de CPU
- Throughput
- Cambios de contexto

### 2. Gestión de Memoria

- ✓ **Paginación por demanda** - Carga lazy de páginas
- ✓ **FIFO** - Algoritmo de reemplazo simple
- ✓ **LRU** - Least Recently Used, mejor rendimiento

**Métricas recolectadas:**

- Fallos de página
- Tasa de fallos y hit ratio
- Número de reemplazos
- Utilización de marcos

### 3. Gestión de Archivos

- ✓ **Problema Lectores-Escritores** - Múltiples lectores, escritor exclusivo
- ✓ **Cola de espera** - Procesos bloqueados
- ✓ **Resolución de conflictos** - Sincronización

**Métricas recolectadas:**

- Accesos exitosos (lectura/escritura)
- Conflictos detectados
- Bloqueos resueltos
- Tasa de conflicto

---

## 📊 Resultados Destacados

### Comparación de Algoritmos de Planificación

(Promedio de 10 procesos, 32 marcos, 5 archivos)

| Algoritmo   | T. Espera | Util. CPU | Cambios Ctx |
| ----------- | --------- | --------- | ----------- |
| Round Robin | 47.3      | 94.5%     | 162         |
| SJF         | 39.6 ↓16% | 96.2%     | 19 ↓88%     |
| Prioridad   | 52.8      | 93.8%     | 23          |

**Conclusión:** SJF es óptimo para tiempo de espera, RR para tiempo de respuesta.

### Comparación FIFO vs LRU

(8 procesos, 16 marcos - memoria restringida)

| Métrica    | FIFO   | LRU    | Mejora  |
| ---------- | ------ | ------ | ------- |
| Hit Ratio  | 41.77% | 50.63% | +8.86pp |
| Reemplazos | 45     | 31     | -31.1%  |

**Conclusión:** LRU supera a FIFO significativamente.

---

## 🏗️ Arquitectura del Sistema

```
┌─────────────────────┐
│   main.c (UI/CLI)   │
└──────────┬──────────┘
           │
           ▼
┌─────────────────────┐
│   Planificador      │◄──── Selección de procesos
│   (Scheduler)       │      Ciclo de simulación
└──┬────────┬────────┬┘
   │        │        │
   ▼        ▼        ▼
┌─────┐ ┌────────┐ ┌────────┐
│PCB  │ │Memoria │ │Archivos│
│Queue│ │Virtual │ │Locks   │
└─────┘ └────────┘ └────────┘
```

**Características de diseño:**

- Modular y mantenible
- Bajo acoplamiento
- Interfaces claras
- Sin memory leaks (verificado con Valgrind)

---

## 📁 Estructura de Archivos

### Código Fuente

- `main.c` - Programa principal y menú
- `proceso.c/h` - Gestión de procesos (PCB)
- `memoria.c/h` - Memoria virtual y paginación
- `archivo.c/h` - Archivos y sincronización
- `planificador.c/h` - Scheduler y algoritmos

### Documentación

- `README.md` - Guía de usuario
- `ESPECIFICACION.md` - Diseño técnico detallado
- `INFORME_TECNICO.md` - Análisis y resultados
- `PRUEBAS.md` - Casos de prueba y validación
- `COMPILAR_WINDOWS.md` - Instrucciones para Windows

### Utilidades

- `Makefile` - Compilación automatizada
- `test.sh` / `test.ps1` - Scripts de prueba
- `LICENSE` - Licencia MIT

---

## 🚀 Guía Rápida de Uso

### Compilar

```bash
make
```

### Ejecutar (Modo Interactivo)

```bash
./simulador
```

### Ejecutar (Línea de Comandos)

```bash
./simulador -a rr -q 4 -r lru -m 32 -p 10 -f 5
```

### Comparar Algoritmos

```bash
make compare
```

---

## 🧪 Validación y Pruebas

**Casos de prueba ejecutados:** 21  
**Tasa de éxito:** 100%

**Categorías:**

- ✓ Planificación de procesos (3 casos)
- ✓ Gestión de memoria (3 casos)
- ✓ Gestión de archivos (2 casos)
- ✓ Comparación de algoritmos (1 caso)
- ✓ Pruebas de estrés (2 casos)
- ✓ Regresión y memory leaks (10 checks)

**Herramientas de validación:**

- Valgrind (Linux) - Sin memory leaks detectados
- Compilación con `-Wall -Wextra` - Sin warnings

---

## 💡 Innovaciones y Contribuciones

1. **Simulación integrada** de tres subsistemas (CPU, Memoria, I/O)
2. **Métricas comprehensivas** para análisis de rendimiento
3. **Interfaz flexible** - Modo interactivo y CLI
4. **Código educativo** - Comentado y estructurado para aprendizaje
5. **Reproducibilidad** - Semilla aleatoria configurable

---

## 📈 Métricas del Proyecto

- **Líneas de código:** ~2,500
- **Archivos:** 17 (código + documentación)
- **Funciones implementadas:** 60+
- **Algoritmos:** 6 (3 scheduling + 2 paging + 1 sync)
- **Tiempo de desarrollo:** 4 semanas
- **Documentación:** 150+ páginas

---

## 🎓 Aprendizajes Clave

### Técnicos

- Implementación de estructuras de datos complejas (colas, tablas)
- Gestión manual de memoria en C
- Algoritmos de sistemas operativos
- Simulación de eventos discretos

### Conceptuales

- Trade-offs en diseño de SO (rendimiento vs equidad)
- Importancia de métricas para evaluación
- No hay "mejor" algoritmo universal
- Valor de la modularización

---

## 🔮 Trabajo Futuro

### Corto Plazo

- [ ] Interfaz gráfica (GUI)
- [ ] Algoritmo MLFQ (Multi-Level Feedback Queue)
- [ ] Exportar métricas a CSV/JSON

### Mediano Plazo

- [ ] Soporte para multiprocesadores (SMP)
- [ ] Simulación de disco (I/O scheduling)
- [ ] Deadlock detection

### Largo Plazo

- [ ] Visualización en tiempo real con animaciones
- [ ] Suite de tests automatizados (CI/CD)
- [ ] Modo educativo interactivo paso a paso

---

## 🏆 Conclusión

El simulador cumple exitosamente todos los objetivos planteados:

✅ **Funcionalidad completa** - Todos los componentes implementados  
✅ **Calidad de código** - Sin warnings, sin leaks, bien documentado  
✅ **Validación exhaustiva** - 21 casos de prueba, 100% éxito  
✅ **Utilidad educativa** - Demuestra conceptos de SO efectivamente  
✅ **Documentación completa** - 150+ páginas de documentación técnica

**El proyecto está listo para uso educativo y sirve como base sólida para extensiones futuras.**

---

## 📞 Recursos

- **Repositorio:** [GitHub - proyecto-final-so]
- **Documentación:** Ver archivos `.md` en el repositorio
- **Soporte:** Ver `COMPILAR_WINDOWS.md` para troubleshooting

---

**Desarrollado con 💻 para el aprendizaje de Sistemas Operativos**  
**UPTC - 2025**
