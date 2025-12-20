@echo off
echo Compilation du solver Wordle...

gcc src\solver_game.c src\main_solver_game.c -o solver.exe

if errorlevel 1 (
    echo ❌ Erreur de compilation
    pause
    exit /b
)

echo ✅ Compilation réussie
solver.exe

pause