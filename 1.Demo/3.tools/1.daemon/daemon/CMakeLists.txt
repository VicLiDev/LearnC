# 添加守护进程可执行文件
add_executable(daemon_guard daemon_guard.c)

# 安装规则
install(TARGETS daemon_guard RUNTIME DESTINATION bin)
