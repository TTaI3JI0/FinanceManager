#!/bin/sh

set -e

SRC="src/main.cpp src/FinanceManager.cpp src/DatabaseManager.cpp src/Utils.cpp libs/sqlite/sqlite3.c"
OUT="FinanceManager"

echo "Compiling $OUT ..."
g++ -std=c++17 -Iinclude -Ilibs/sqlite $SRC -o "$OUT"
echo "Build succeeded."

