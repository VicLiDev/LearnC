#!/usr/bin/env bash
#########################################################################
# File Name: setup_repo_prj.sh
# Author: LiHongjin
# mail: 872648180@qq.com
# Created Time: Fri 01 Nov 2024 03:04:50 PM CST
#########################################################################

# 定义仓库目录
REPO_DIR="demo-repo"

# 如果目录存在，则删除它
if [ -d "$REPO_DIR" ]; then
    echo "Removing existing directory: $REPO_DIR"
    rm -rf "$REPO_DIR"
fi

# 创建一个新的 Git 仓库
mkdir "$REPO_DIR"
cd "$REPO_DIR"
git init

# 创建一个主分支并添加初始文件
echo "Hello World" > README.md
git add README.md
git commit -m "master: Initial commit"

# 在主分支上添加几个提交
echo "First update in master branch" >> README.md
git add README.md
git commit -m "master: First update in master branch"

echo "Second update in master branch" >> README.md
git add README.md
git commit -m "master: Second update in master branch"

# 创建多个特性分支及其子分支
for i in {1..4}; do
    git checkout -b feature-$i
    echo "Feature $i work" > feature$i.txt
    git add feature$i.txt
    git commit -m "feature-$i: Add feature $i"
    
    # 在每个特性分支上添加多个提交
    for j in {1..3}; do
        echo "Feature $i update $j" >> feature$i.txt
        git add feature$i.txt
        git commit -m "feature-$i: Update feature $i - part $j"
    done
    
    # 创建子分支
    git checkout -b feature-$i.1
    echo "Feature $i.1 work" > feature${i}_1.txt
    git add feature${i}_1.txt
    git commit -m "feature-$i.1: Work on feature $i.1"

    for k in {1..2}; do
        echo "Feature $i.1 update $k" >> feature${i}_1.txt
        git add feature${i}_1.txt
        git commit -m "feature-$i.1: Update feature $i.1 - part $k"
    done
    
    # 返回主分支
    git checkout master
done

# 返回主分支并添加不同的更新
echo "Final update in master branch" >> README.md
git add README.md
git commit -m "master: Final update in master branch"

# 合并特性分支及其子分支到主分支
for i in {1..4}; do
    git checkout master
    git merge feature-$i -m "master: Merge feature-$i into master"
    
    # 合并子分支
    git merge feature-$i.1 -m "master: Merge feature-$i.1 into master"
done

# 展示提交历史
git log --graph --oneline --all

# 完成
echo "Demo repository with branches and sub-branches created and merged successfully."
