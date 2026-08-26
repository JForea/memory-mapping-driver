add_executable(
    mp-alloc-test
    src/main.cpp
)

target_include_directories(
    mp-alloc-test
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)