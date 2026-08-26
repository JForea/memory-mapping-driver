cmake_minimum_required(VERSION 3.10.0)
project(memory-mapping-driver-samples VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(null_mapping)
add_subdirectory(mp_alloc_test)
