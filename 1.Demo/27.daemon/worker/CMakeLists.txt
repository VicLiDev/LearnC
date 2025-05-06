# 添加可执行文件
add_executable(test_worker worker.cpp)

# 链接mlib库
target_link_libraries(test_worker PRIVATE mlib mlibcpp)

# 设置包含目录
target_include_directories(test_worker PRIVATE
    ${CMAKE_SOURCE_DIR}/mlib
    ${CMAKE_SOURCE_DIR}/mlibcpp
)

# 设置运行时库路径
set_target_properties(test_worker PROPERTIES
    INSTALL_RPATH "$ORIGIN/../lib"
    BUILD_WITH_INSTALL_RPATH TRUE
)

# 安装规则
install(TARGETS test_worker RUNTIME DESTINATION bin)



# 添加可执行文件
add_executable(test_worker2 worker2.cpp)

# 链接mlib库
target_link_libraries(test_worker2 PRIVATE mlib mlibcpp)

# 设置包含目录
target_include_directories(test_worker2 PRIVATE
    ${CMAKE_SOURCE_DIR}/mlib
    ${CMAKE_SOURCE_DIR}/mlibcpp
)

# 设置运行时库路径
set_target_properties(test_worker2 PROPERTIES
    INSTALL_RPATH "$ORIGIN/../lib"
    BUILD_WITH_INSTALL_RPATH TRUE
)

# 安装规则
install(TARGETS test_worker2 RUNTIME DESTINATION bin)
