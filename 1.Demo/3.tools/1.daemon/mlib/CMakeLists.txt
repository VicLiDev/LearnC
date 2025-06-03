# 创建共享库
add_library(mlib SHARED mlib.c mlib.h)

# 设置库版本
set_target_properties(mlib PROPERTIES
    VERSION 1.0.0
    SOVERSION 1
)

# 安装规则
install(TARGETS mlib
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(FILES mlib.h DESTINATION include/mlib)
