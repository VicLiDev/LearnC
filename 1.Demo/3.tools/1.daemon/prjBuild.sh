#!/usr/bin/env bash
#########################################################################
# File Name: prjBuild.sh
# Author: LiHongjin
# mail: 872648180@qq.com
# Created Time: Mon 05 May 2025 11:10:40 AM CST
#########################################################################

root_dir=`pwd`
build_dir="build"

[ -e ${build_dir} ] && rm -rf ${build_dir}
mkdir ${build_dir} && cd ${build_dir} && cmake .. && make

# run

${root_dir}/${build_dir}/bin/daemon_guard \
    --cmd "${root_dir}/${build_dir}/bin/test_worker" \
    --name "work1" \
    --max-restarts 5 \
    --restart-delay 3 \
    -- \
    --cmd "${root_dir}/${build_dir}/bin/test_worker2" \
    --name "work2" \
    --max-restarts 3

# killall /home/lhj/Projects/LearnC/1.Demo/27.daemon/build/bin/daemon
# killall /home/lhj/Projects/LearnC/1.Demo/27.daemon/build/bin/worker
