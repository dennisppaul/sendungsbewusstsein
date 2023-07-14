cmake_minimum_required(VERSION 3.25)
project(sendungsbewusstsein)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

# simpleble
include_directories(simpleble/include)
link_directories(simpleble/lib)

# oscpack
include_directories(oscpack/include)
include_directories(oscpack/include/ip)
include_directories(oscpack/include/osc)
FILE(GLOB_RECURSE OSCPACK_SRC oscpack/src/*.cpp)

# cxxopts
include_directories(cxxopts/include)

# application
include_directories(include)
add_executable(sendungsbewusstsein
        src/main.cpp
        src/utils.cpp
        src/OscSenderReceiver.cpp
        src/MOscPacketListener.cpp
        ${OSCPACK_SRC}
)
set_target_properties(sendungsbewusstsein PROPERTIES LINK_FLAGS "-Wl,-S")

# libraries
target_compile_definitions(sendungsbewusstsein PRIVATE SIMPLEBLE_STATIC_DEFINE)
target_link_libraries(sendungsbewusstsein libsimpleble.a "-framework CoreBluetooth" "-framework Foundation")
