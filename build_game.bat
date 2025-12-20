@echo off
echo Compilation du jeu Wordle...

gcc src\wordle_game.c  -o wordle.exe

if errorlevel 1 (
    echo ❌ Erreur de compilation
    pause
    exit /b
)

echo ✅ Compilation réussie
echo Lancement du jeu...
wordle.exe

pause