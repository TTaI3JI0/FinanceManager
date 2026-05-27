@echo off
setlocal

set SRC=src\main.cpp src\FinanceManager.cpp
set OUT=FinanceManager.exe

echo Compiling %OUT% ...
g++ -std=c++17 -Iinclude %SRC% -o %OUT%

if %ERRORLEVEL% NEQ 0 (
    echo Build failed.
    exit /b 1
) else (
    echo Build succeeded.
)

endlocal

