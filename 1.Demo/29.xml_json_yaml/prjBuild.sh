#!/usr/bin/env bash
#########################################################################
# File Name: prjBuild.sh
# Author: LiHongjin
# mail: 872648180@qq.com
# Created Time: Tue 27 May 2025 03:25:21 PM CST
#########################################################################

cmake -B build -DCMAKE_BUILD_TYPE=Release && make -C build

./build/json_demo_c
./build/json_demo_cpp
./build/xml_demo_c
./build/xml_demo_cpp
./build/yaml_demo_c
./build/yaml_demo_cpp
