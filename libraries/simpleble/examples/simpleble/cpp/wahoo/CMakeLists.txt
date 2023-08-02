cmake_minimum_required(VERSION 3.21)

project(EXAMPLE_WAHOO)

message("-- [INFO] Building Example (wahoo)")

add_executable(example_wahoo
    wahoo.cpp
    ../common/utils.cpp
)

target_link_libraries(example_wahoo simpleble::simpleble)
