# Script de prueba para Windows PowerShell
# Ejecutar con: .\test.ps1

Write-Host "================================" -ForegroundColor Cyan
Write-Host "PRUEBAS DEL SIMULADOR DE SO" -ForegroundColor Cyan
Write-Host "================================" -ForegroundColor Cyan
Write-Host ""

# Compilar
Write-Host "[1/4] Compilando..." -ForegroundColor Yellow
if (Test-Path "simulador.exe") {
    Remove-Item "simulador.exe"
}
if (Test-Path "*.o") {
    Remove-Item "*.o"
}

gcc -o simulador.exe main.c proceso.c memoria.c archivo.c planificador.c -std=c99 -Wall -Wextra

if ($LASTEXITCODE -eq 0) {
    Write-Host "✓ Compilación exitosa" -ForegroundColor Green
}
else {
    Write-Host "✗ ERROR: Falló la compilación" -ForegroundColor Red
    exit 1
}
Write-Host ""

# Prueba 1: Round Robin
Write-Host "[2/4] Probando Round Robin..." -ForegroundColor Yellow
.\simulador.exe -a rr -q 4 -r lru -m 32 -p 5 -f 3 > test_rr.txt 2>&1
if ($LASTEXITCODE -eq 0) {
    Write-Host "✓ Round Robin funciona" -ForegroundColor Green
}
else {
    Write-Host "✗ Round Robin falló" -ForegroundColor Red
}
Write-Host ""

# Prueba 2: SJF
Write-Host "[3/4] Probando SJF..." -ForegroundColor Yellow
.\simulador.exe -a sjf -r lru -m 32 -p 5 -f 3 > test_sjf.txt 2>&1
if ($LASTEXITCODE -eq 0) {
    Write-Host "✓ SJF funciona" -ForegroundColor Green
}
else {
    Write-Host "✗ SJF falló" -ForegroundColor Red
}
Write-Host ""

# Prueba 3: Prioridad
Write-Host "[4/4] Probando Prioridad..." -ForegroundColor Yellow
.\simulador.exe -a prioridad -r lru -m 32 -p 5 -f 3 > test_prio.txt 2>&1
if ($LASTEXITCODE -eq 0) {
    Write-Host "✓ Prioridad funciona" -ForegroundColor Green
}
else {
    Write-Host "✗ Prioridad falló" -ForegroundColor Red
}
Write-Host ""

Write-Host "================================" -ForegroundColor Cyan
Write-Host "PRUEBAS COMPLETADAS" -ForegroundColor Cyan
Write-Host "================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Los resultados están en test_*.txt" -ForegroundColor Yellow
Write-Host "Ejecuta 'Get-Content test_rr.txt' para ver la salida" -ForegroundColor Yellow
