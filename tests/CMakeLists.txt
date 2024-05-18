cmake_minimum_required(VERSION 3.24)
project(tests)

set(CMAKE_CXX_STANDARD 14)

# Fetch Catch2 from the repository
include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.4.0
)
FetchContent_MakeAvailable(Catch2)

add_executable(tests main_test.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain lisp_structure_lib)
target_include_directories(tests PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Define custom target to run tests easily
add_custom_target(run_tests
    COMMAND tests
    DEPENDS tests
)