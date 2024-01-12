cmake_minimum_required(VERSION 3.25)

project(sendungsbewusstsein)

if(WIN32)
    set(CMAKE_SYSTEM_VERSION 10.0)
endif()

set(CMAKE_CXX_STANDARD 20)
if (APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "Build architectures for Mac OS X" FORCE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif ()
set(SB_LIBRARY_PATH "libraries")
add_executable(sendungsbewusstsein)

# simpleble
add_subdirectory(${SB_LIBRARY_PATH}/simpleble/simpleble ${CMAKE_BINARY_DIR}/simpleble)
target_link_libraries(sendungsbewusstsein simpleble::simpleble)

# oscpack
target_include_directories(sendungsbewusstsein PRIVATE ${SB_LIBRARY_PATH}/oscpack/include)
target_include_directories(sendungsbewusstsein PRIVATE ${SB_LIBRARY_PATH}/oscpack/include/ip)
target_include_directories(sendungsbewusstsein PRIVATE ${SB_LIBRARY_PATH}/oscpack/include/osc)
if (APPLE OR UNIX)
    file(GLOB OSCPACK_SRC
        ${SB_LIBRARY_PATH}/oscpack/src/*.cpp
        ${SB_LIBRARY_PATH}/oscpack/src/ip/*.cpp
        ${SB_LIBRARY_PATH}/oscpack/src/ip/posix/*.cpp
        ${SB_LIBRARY_PATH}/oscpack/src/osc/*.cpp
    )
elseif (WIN32)
    file(GLOB OSCPACK_SRC
        ${SB_LIBRARY_PATH}/oscpack/src/*.cpp
        ${SB_LIBRARY_PATH}/oscpack/src/ip/*.cpp
        ${SB_LIBRARY_PATH}/oscpack/src/ip/win32/*.cpp
        ${SB_LIBRARY_PATH}/oscpack/src/osc/*.cpp
)
else ()
    file(GLOB OSCPACK_SRC
        ${SB_LIBRARY_PATH}/oscpack/src/*.cpp
        ${SB_LIBRARY_PATH}/oscpack/src/ip/*.cpp
        ${SB_LIBRARY_PATH}/oscpack/src/ip/posix/*.cpp
        ${SB_LIBRARY_PATH}/oscpack/src/osc/*.cpp
    )
endif ()

# cxxopts
target_include_directories(sendungsbewusstsein PRIVATE ${SB_LIBRARY_PATH}/cxxopts/include)

# application
target_include_directories(sendungsbewusstsein PRIVATE include)
target_sources(sendungsbewusstsein PRIVATE
        src/Console.cpp
        src/utils.cpp
        src/Transceiver.cpp
        src/TransceiverNull.cpp
        src/TransceiverOSC.cpp
        src/StringUtils.cpp
        src/main.cpp
        ${OSCPACK_SRC}
)
set_target_properties(sendungsbewusstsein PROPERTIES LINK_FLAGS "-Wl,-S")

# libraries
target_compile_definitions(sendungsbewusstsein PRIVATE SIMPLEBLE_STATIC_DEFINE)

if (APPLE)
    target_link_libraries(sendungsbewusstsein "-framework CoreBluetooth" "-framework Foundation")
elseif(WIN32)
    target_link_libraries(sendungsbewusstsein windowsapp)
endif()
