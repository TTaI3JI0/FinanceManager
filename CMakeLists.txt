cmake_minimum_required(VERSION 4.0)
project(FinanceManager)

set(CMAKE_CXX_STANDARD 20)

add_executable(FinanceManager
        src/main.cpp
        src/FinanceManager.cpp
)
include_directories(include)
