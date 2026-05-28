@echo off
setlocal

set SRC=src\main.cpp src\FinanceManager.cpp src\DatabaseManager.cpp src\Utils.cpp libs\sqlite\sqlite3.c
set OUT=FinanceManager.exe

echo Compiling %OUT% ...
g++ -std=c++17 -Iinclude -Ilibs\sqlite %SRC% -o %OUT%

if %ERRORLEVEL% NEQ 0 (
    echo Build failed.
    exit /b 1
) else (
    echo Build succeeded.
)

endlocal

