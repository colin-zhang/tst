cmake_minimum_required(VERSION 3.10)
project(c__11)

set(CMAKE_CXX_STANDARD 11)

add_executable(yeild yeild.cpp)
add_executable(future future.cpp )
add_executable(promise1 promise.cpp)
add_executable(thread thread.cpp)
add_executable(class class.cpp)

target_link_libraries(future pthread)
target_link_libraries(promise1 pthread)
target_link_libraries(thread pthread)


