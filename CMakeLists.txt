cmake_minimum_required(VERSION 2.8.11)

project(top100)

enable_testing()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${top100_SOURCE_DIR}/bin)

add_subdirectory(src)
add_subdirectory(tests)
