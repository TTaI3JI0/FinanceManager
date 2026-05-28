cmake_minimum_required(VERSION 4.0)
project(FinanceManager)

set(CMAKE_CXX_STANDARD 20)

add_executable(FinanceManager
        src/main.cpp
        src/FinanceManager.cpp
        src/DatabaseManager.cpp
        src/Utils.cpp
        libs/sqlite/sqlite3.c
)
target_include_directories(FinanceManager PRIVATE include libs/sqlite)
