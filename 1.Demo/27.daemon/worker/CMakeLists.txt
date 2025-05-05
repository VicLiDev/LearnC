# 添加可执行文件
add_executable(worker worker.cpp)

# 链接mlib库
target_link_libraries(worker PRIVATE mlib mlibcpp)

# 设置包含目录
target_include_directories(worker PRIVATE
    ${CMAKE_SOURCE_DIR}/mlib
    ${CMAKE_SOURCE_DIR}/mlibcpp
)

# 设置运行时库路径
set_target_properties(worker PROPERTIES
    INSTALL_RPATH "$ORIGIN/../lib"
    BUILD_WITH_INSTALL_RPATH TRUE
)

# 安装规则
install(TARGETS worker RUNTIME DESTINATION bin)
