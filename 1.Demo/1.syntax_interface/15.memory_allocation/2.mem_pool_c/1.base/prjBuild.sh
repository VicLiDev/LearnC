#!/usr/bin/env bash
#########################################################################
# File Name: prjBuild.sh
# Author: LiHongjin
# mail: 872648180@qq.com
# Created Time: Tue 24 Jun 2025 06:08:56 PM CST
#########################################################################

echo "======> build <======"
make
echo
echo "======> test <======"
make test
echo
echo "======> clean <======"
make clean
echo
