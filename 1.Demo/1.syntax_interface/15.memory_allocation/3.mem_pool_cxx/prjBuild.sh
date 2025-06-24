#!/usr/bin/env bash
#########################################################################
# File Name: prjBuild.sh
# Author: LiHongjin
# mail: 872648180@qq.com
# Created Time: Tue 24 Jun 2025 08:23:34 PM CST
#########################################################################

build_dir="build"

[ -e ${build_dir} ] && rm -rf ${build_dir}
mkdir ${build_dir}

cd ${build_dir} && cmake .. && make && ./test && cd .. && rm -rf ${build_dir}
