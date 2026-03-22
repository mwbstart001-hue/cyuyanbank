cmake_minimum_required(VERSION 3.14)
project(BankManagementSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(SOURCES
    src/main.cpp
    src/common.cpp
    src/queuing_system.cpp
    src/account_relationship.cpp
    src/transaction_flow.cpp
    src/risk_assessment.cpp
    src/file_storage.cpp
)

add_executable(bank_system ${SOURCES})