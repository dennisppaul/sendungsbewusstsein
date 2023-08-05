cmake_minimum_required(VERSION 3.25)
project(sendungsbewusstsein)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
set(SB_LIBRARY_PATH "libraries")
add_executable(sendungsbewusstsein)

# simpleble
add_subdirectory(${SB_LIBRARY_PATH}/simpleble/simpleble ${CMAKE_BINARY_DIR}/simpleble)
target_link_libraries(sendungsbewusstsein simpleble::simpleble)

# oscpack
target_include_directories(sendungsbewusstsein PRIVATE ${SB_LIBRARY_PATH}/oscpack/include)
target_include_directories(sendungsbewusstsein PRIVATE ${SB_LIBRARY_PATH}/oscpack/include/ip)
target_include_directories(sendungsbewusstsein PRIVATE ${SB_LIBRARY_PATH}/oscpack/include/osc)
FILE(GLOB_RECURSE OSCPACK_SRC ${SB_LIBRARY_PATH}/oscpack/src/*.cpp)

# cxxopts
target_include_directories(sendungsbewusstsein PRIVATE ${SB_LIBRARY_PATH}/cxxopts/include)

# application
target_include_directories(sendungsbewusstsein PRIVATE include)
target_sources(sendungsbewusstsein PRIVATE
        src/Console.cpp
        src/utils.cpp
        src/Transceiver.cpp
        src/OSCListener.cpp
        src/main.cpp
        src/StringUtils.cpp
        ${OSCPACK_SRC}
)
set_target_properties(sendungsbewusstsein PROPERTIES LINK_FLAGS "-Wl,-S")

# libraries
target_compile_definitions(sendungsbewusstsein PRIVATE SIMPLEBLE_STATIC_DEFINE)
target_link_libraries(sendungsbewusstsein "-framework CoreBluetooth" "-framework Foundation")
