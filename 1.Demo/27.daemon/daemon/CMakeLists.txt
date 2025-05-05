# 添加守护进程可执行文件
add_executable(daemon daemon.c)

# 安装规则
install(TARGETS daemon RUNTIME DESTINATION bin)
