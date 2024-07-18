SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
# 或者在命令行加上 cmake -DCMAKE_CXX_FLAGS=-fsanitize=address ..

ADD_EXECUTABLE(asan_demo_cmk)
TARGET_SOURCES(asan_demo_cmk PRIVATE asan_demo.cpp)
