cmake_minimum_required(VERSION 3.10)
project(MemoryPool)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_library(memory_pool
    src/MemoryPool.cpp
)

add_executable(test test/test.cpp)
target_link_libraries(test memory_pool)
target_compile_options(test PRIVATE -rdynamic)

