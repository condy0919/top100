set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${top100_SOURCE_DIR}/bin/tests)

find_package(Threads REQUIRED)

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

add_executable(cqueue cqueue.cpp)
target_link_libraries(cqueue ${CMAKE_THREAD_LIBS_INIT})
add_test(NAME cqueue COMMAND cqueue)

add_executable(thread_pool thread_pool.cpp)
target_link_libraries(thread_pool ${CMAKE_THREAD_LIBS_INIT})
add_test(NAME thread_pool COMMAND thread_pool)
