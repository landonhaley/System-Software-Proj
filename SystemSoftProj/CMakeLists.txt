cmake_minimum_required(VERSION 3.3)
project(SystemSoftProj)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

##TODO: Put vm.c and vm.h back in here when done working on the scanner.
set(SOURCE_FILES scanner.c scanner.h symbolTable.c symbolTable.h)
add_executable(SystemSoftProj ${SOURCE_FILES})