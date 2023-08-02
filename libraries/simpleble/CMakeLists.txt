cmake_minimum_required(VERSION 3.26)
project(test-simpleble)

set(CMAKE_CXX_STANDARD 20)

link_directories(lib)

include_directories(include)
include_directories(test)

add_executable(simpleble test/connect.cpp test/utils.cpp)

target_compile_definitions(simpleble PRIVATE SIMPLEBLE_STATIC_DEFINE)

target_link_libraries(simpleble libsimpleble.a "-framework CoreBluetooth" "-framework Foundation")
