set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${top100_SOURCE_DIR}/bin/tests)

include_directories(${top100_SOURCE_DIR}/src)
add_definitions(-std=c++2a)
add_definitions(-pipe)
add_definitions(-Wall)
add_definitions(-Wextra)
add_definitions(-g -ggdb)

add_executable(topn topn.cpp)
target_link_libraries(topn)
add_test(NAME topn COMMAND topn)

add_executable(noncopyable_function noncopyable_function.cpp)
target_link_libraries(noncopyable_function)
add_test(NAME noncopyable_function COMMAND noncopyable_function)
