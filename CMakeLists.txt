cmake_minimum_required(VERSION 3.24)
project(LispStructure)

set(CMAKE_CXX_STANDARD 14)

# Add all the sources to a library
add_library(lisp_structure_lib STATIC
    src/Atom.cpp
    src/AtomMemento.cpp
    src/LispElement.cpp
    src/Memento.cpp
    src/TransactionToken.cpp
    src/List.cpp
    src/ListMemento.cpp
)

# Include headers for the library
target_include_directories(lisp_structure_lib PUBLIC include)

# Add the subdirectory containing tests
add_subdirectory(tests)