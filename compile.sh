#!/bin/sh

set -e

SRC="src/main.cpp src/FinanceManager.cpp"
OUT="FinanceManager"

echo "Compiling $OUT ..."
g++ -std=c++17 -Iinclude $SRC -o "$OUT"
echo "Build succeeded."

