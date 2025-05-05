cmake_minimum_required(VERSION 3.10)
project(ProcessMonitorSystem)

# 设置C标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 添加包含目录
include_directories(
    ${CMAKE_SOURCE_DIR}/mlib
    ${CMAKE_SOURCE_DIR}/mlibcpp
)

# 添加子目录
add_subdirectory(mlib)
add_subdirectory(mlibcpp)
add_subdirectory(worker)
add_subdirectory(daemon)

# 安装规则
install(DIRECTORY doc/ DESTINATION share/doc/process-monitor)
install(DIRECTORY mlib/ DESTINATION include/mlib FILES_MATCHING PATTERN "*.h")
install(DIRECTORY mlibcpp/ DESTINATION include/mlibcpp FILES_MATCHING PATTERN "*.h")
