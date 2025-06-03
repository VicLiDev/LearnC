# 创建共享库
add_library(mlibcpp SHARED mlibcpp.cpp mlibcpp.h)

# 设置库版本
set_target_properties(mlibcpp PROPERTIES
    VERSION 1.0.0
    SOVERSION 1
)

# 安装规则
install(TARGETS mlibcpp
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(FILES mlibcpp.h DESTINATION include/mlibcpp)
