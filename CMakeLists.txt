cmake_minimum_required(VERSION 3.24)
project(LispStructure)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)
add_executable(LispStructure main.cpp include/LispElement.h include/Memento.h include/TransactionToken.h include/AtomMemento.h include/Atom.h src/Atom.cpp src/AtomMemento.cpp src/LispElement.cpp src/Memento.cpp src/TransactionToken.cpp include/ListMemento.h include/List.h src/List.cpp src/ListMemento.cpp)

add_subdirectory(tests)
