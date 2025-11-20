# Instrucciones de Compilación y Ejecución para Windows

## Requisitos Previos

### 1. Instalar MinGW (Compilador GCC para Windows)

**Opción A: MinGW-w64**

1. Descargar desde: https://www.mingw-w64.org/downloads/
2. O usar MSYS2: https://www.msys2.org/
3. Agregar al PATH: `C:\msys64\mingw64\bin`

**Opción B: TDM-GCC**

1. Descargar desde: https://jmeubank.github.io/tdm-gcc/
2. Instalar con opciones por defecto
3. Automáticamente se agrega al PATH

**Verificar instalación:**

```powershell
gcc --version
```

Debería mostrar algo como:

```
gcc (MinGW-W64 x86_64) 11.2.0
```

### 2. Make (Opcional pero Recomendado)

**Con MSYS2:**

```powershell
pacman -S make
```

**Alternativa sin Make:** Usar comandos GCC directamente (ver abajo)

---

## Compilación

### Opción 1: Con Make

```powershell
# Compilar
make

# Limpiar
make clean

# Recompilar
make rebuild
```

### Opción 2: Sin Make (Comando Directo)

```powershell
gcc -o simulador.exe main.c proceso.c memoria.c archivo.c planificador.c -std=c99 -Wall -Wextra
```

Si hay errores de compilación, intentar con:

```powershell
gcc -o simulador.exe main.c proceso.c memoria.c archivo.c planificador.c -std=c99
```

---

## Ejecución

### Modo Interactivo

```powershell
.\simulador.exe
```

### Modo Línea de Comandos

**Ejemplos:**

```powershell
# Round Robin con quantum 5
.\simulador.exe -a rr -q 5 -r lru -m 32 -p 10 -f 5

# Shortest Job First
.\simulador.exe -a sjf -r lru -m 64 -p 15 -f 8

# Prioridad con FIFO
.\simulador.exe -a prioridad -r fifo -m 32 -p 12 -f 6
```

### Pruebas Rápidas

**Si tienes Make:**

```powershell
# Simulación rápida
make quick

# Comparar algoritmos
make compare

# Comparar FIFO vs LRU
make test-memory
```

**Sin Make:**

```powershell
# Simulación rápida
.\simulador.exe -a rr -q 4 -r lru -m 32 -p 10 -f 5

# SJF
.\simulador.exe -a sjf -r lru -m 32 -p 10 -f 5

# Prioridad
.\simulador.exe -a prioridad -r lru -m 32 -p 10 -f 5
```

---

## Solución de Problemas Comunes

### Error: 'gcc' no se reconoce

**Solución:** Agregar GCC al PATH

1. Buscar la carpeta de instalación de MinGW (ej: `C:\MinGW\bin`)
2. Abrir "Variables de entorno" en Windows
3. Editar la variable PATH
4. Agregar la ruta del bin de MinGW
5. Reiniciar PowerShell/CMD

### Error: 'make' no se reconoce

**Solución 1:** Instalar Make con MSYS2

```powershell
pacman -S make
```

**Solución 2:** Usar comandos GCC directamente (ver arriba)

### Warnings sobre \_CRT_SECURE_NO_WARNINGS

**Solución:** Agregar flag al compilar:

```powershell
gcc -o simulador.exe main.c proceso.c memoria.c archivo.c planificador.c -std=c99 -D_CRT_SECURE_NO_WARNINGS
```

O editar el Makefile:

```makefile
CFLAGS = -std=c99 -Wall -Wextra -O2 -D_CRT_SECURE_NO_WARNINGS
```

### El programa se cierra inmediatamente

**Causa:** Probablemente errores de runtime

**Solución:** Ejecutar desde PowerShell/CMD para ver mensajes de error:

```powershell
.\simulador.exe
# Presiona Enter y observa mensajes
```

### Caracteres raros en la salida (�)

**Causa:** Problema de codificación en terminal Windows

**Solución:** Cambiar codificación a UTF-8

```powershell
chcp 65001
.\simulador.exe
```

O modificar el código para no usar caracteres especiales.

---

## Compilación Optimizada para Release

Para mejor rendimiento:

```powershell
gcc -o simulador.exe main.c proceso.c memoria.c archivo.c planificador.c -std=c99 -O3 -DNDEBUG
```

Flags:

- `-O3`: Optimización máxima
- `-DNDEBUG`: Desactiva asserts (más rápido)

---

## Verificar Memoria (Opcional)

### Con Dr. Memory (Windows)

1. Descargar: https://drmemory.org/
2. Instalar
3. Ejecutar:

```powershell
drmemory.exe -- .\simulador.exe -a rr -q 4 -r lru -m 16 -p 5 -f 3
```

---

## Ejecución desde Visual Studio Code

### 1. Abrir carpeta del proyecto en VS Code

### 2. Instalar extensión "C/C++" de Microsoft

### 3. Crear configuración de compilación

Archivo: `.vscode/tasks.json`

```json
{
  "version": "2.0.0",
  "tasks": [
    {
      "label": "Compilar Simulador",
      "type": "shell",
      "command": "gcc",
      "args": [
        "-o",
        "simulador.exe",
        "main.c",
        "proceso.c",
        "memoria.c",
        "archivo.c",
        "planificador.c",
        "-std=c99",
        "-Wall",
        "-Wextra"
      ],
      "group": {
        "kind": "build",
        "isDefault": true
      }
    }
  ]
}
```

### 4. Compilar

Presionar `Ctrl+Shift+B`

### 5. Ejecutar

Abrir terminal integrada (`Ctrl+ñ`) y ejecutar:

```powershell
.\simulador.exe
```

---

## Makefile Alternativo para Windows (sin POSIX)

Si el Makefile incluido no funciona, crear `Makefile.win`:

```makefile
CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -O2
TARGET = simulador.exe
SOURCES = main.c proceso.c memoria.c archivo.c planificador.c
OBJECTS = main.o proceso.o memoria.o archivo.o planificador.o

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del /Q $(OBJECTS) $(TARGET)

.PHONY: all clean
```

Usar con:

```powershell
make -f Makefile.win
```

---

## Script de Compilación Automático (PowerShell)

Guardar como `compilar.ps1`:

```powershell
# Script de compilación para Windows
Write-Host "Compilando Simulador de SO..." -ForegroundColor Green

$sources = @("main.c", "proceso.c", "memoria.c", "archivo.c", "planificador.c")
$output = "simulador.exe"
$flags = "-std=c99", "-Wall", "-Wextra", "-O2"

# Verificar GCC
if (-not (Get-Command gcc -ErrorAction SilentlyContinue)) {
    Write-Host "ERROR: GCC no encontrado. Por favor instale MinGW." -ForegroundColor Red
    exit 1
}

# Compilar
Write-Host "Ejecutando: gcc $flags -o $output $sources" -ForegroundColor Yellow
gcc @flags -o $output @sources

if ($LASTEXITCODE -eq 0) {
    Write-Host "Compilación exitosa!" -ForegroundColor Green
    Write-Host "Ejecutar con: .\$output" -ForegroundColor Cyan
} else {
    Write-Host "Error en la compilación." -ForegroundColor Red
    exit 1
}
```

Ejecutar:

```powershell
.\compilar.ps1
```

Si hay error de ejecución de scripts:

```powershell
Set-ExecutionPolicy -Scope CurrentUser -ExecutionPolicy RemoteSigned
.\compilar.ps1
```

---

## Resumen de Comandos Rápidos

```powershell
# 1. Compilar
gcc -o simulador.exe main.c proceso.c memoria.c archivo.c planificador.c -std=c99

# 2. Ejecutar interactivo
.\simulador.exe

# 3. Ejecutar con parámetros
.\simulador.exe -a rr -q 4 -r lru -m 32 -p 10 -f 5

# 4. Limpiar archivos compilados
del simulador.exe *.o
```

---

## Contacto y Soporte

Si tienes problemas con la compilación o ejecución:

1. Verifica que GCC esté instalado: `gcc --version`
2. Asegúrate de estar en la carpeta correcta del proyecto
3. Revisa que todos los archivos .c y .h estén presentes
4. Lee los mensajes de error completos
5. Consulta la documentación de MinGW

---

**¡Listo para ejecutar el simulador en Windows!** 🚀
