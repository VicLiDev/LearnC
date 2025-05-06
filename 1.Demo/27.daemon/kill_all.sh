#!/usr/bin/env bash
#########################################################################
# File Name: kill_all.sh
# Author: LiHongjin
# mail: 872648180@qq.com
# Created Time: Mon 05 May 2025 11:45:49 AM CST
#########################################################################

# killall /home/lhj/Projects/LearnC/1.Demo/27.daemon/build/bin/daemon
# killall /home/lhj/Projects/LearnC/1.Demo/27.daemon/build/bin/worker

# killall daemon_guard
# killall worker

# xargs 的主要作用是从标准输入读取数据，并将这些数据作为参数传递给其他命令。它的典型使用场景包括：
pgrep -f daemon_guard | xargs sudo kill
pgrep -f test_worker | xargs sudo kill
