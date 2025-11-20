#!/bin/bash
# Script de prueba rápida del simulador

echo "================================"
echo "PRUEBAS DEL SIMULADOR DE SO"
echo "================================"
echo ""

# Compilar
echo "[1/4] Compilando..."
make clean > /dev/null 2>&1
make
if [ $? -ne 0 ]; then
    echo "ERROR: Falló la compilación"
    exit 1
fi
echo "✓ Compilación exitosa"
echo ""

# Prueba 1: Round Robin
echo "[2/4] Probando Round Robin..."
./simulador -a rr -q 4 -r lru -m 32 -p 5 -f 3 > /tmp/test_rr.txt
if [ $? -eq 0 ]; then
    echo "✓ Round Robin funciona"
else
    echo "✗ Round Robin falló"
fi
echo ""

# Prueba 2: SJF
echo "[3/4] Probando SJF..."
./simulador -a sjf -r lru -m 32 -p 5 -f 3 > /tmp/test_sjf.txt
if [ $? -eq 0 ]; then
    echo "✓ SJF funciona"
else
    echo "✗ SJF falló"
fi
echo ""

# Prueba 3: Prioridad
echo "[4/4] Probando Prioridad..."
./simulador -a prioridad -r lru -m 32 -p 5 -f 3 > /tmp/test_prio.txt
if [ $? -eq 0 ]; then
    echo "✓ Prioridad funciona"
else
    echo "✗ Prioridad falló"
fi
echo ""

echo "================================"
echo "PRUEBAS COMPLETADAS"
echo "================================"
echo ""
echo "Los resultados están en /tmp/test_*.txt"
echo "Ejecuta 'cat /tmp/test_rr.txt' para ver la salida"
