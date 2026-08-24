add_executable(
    null-mapping
    src/main.cpp
)

target_include_directories(
    null-mapping
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)