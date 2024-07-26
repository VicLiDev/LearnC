SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
# 或者在命令行加上 cmake -DCMAKE_CXX_FLAGS=-fsanitize=address ..
# 或者在命令行加上 cmake -DCMAKE_C_FLAGS=-fsanitize=address ..

ADD_EXECUTABLE(asan_demo_cmk)
TARGET_SOURCES(asan_demo_cmk PRIVATE asan_demo.c)
