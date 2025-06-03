#!/usr/bin/env bash
#########################################################################
# File Name: setup_repo.sh
# Author: LiHongjin
# mail: 872648180@qq.com
# Created Time: Fri 01 Nov 2024 02:27:20 PM CST
#########################################################################

# 脚本工作的逻辑可以直接看注释，可以看明白

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

echo "Third update in master branch" >> README.md
git add README.md
git commit -m "master: Third update in master branch"

# 创建第一个分支并做一些更改
git checkout -b feature-1
echo "Feature 1 added" > feature1.txt
git add feature1.txt
git commit -m "feature-1: Add feature 1"

# 在 feature-1 上增加更多提交
echo "Feature 1 update 1" >> feature1.txt
git add feature1.txt
git commit -m "feature-1: Update feature 1 - part 1"

echo "Feature 1 update 2" >> feature1.txt
git add feature1.txt
git commit -m "feature-1: Update feature 1 - part 2"

# 创建一个新的分支 feature-1.1 从 feature-1
git checkout -b feature-1.1
echo "Feature 1.1 work" > feature1_1.txt
git add feature1_1.txt
git commit -m "feature-1.1: Work on feature 1.1"

# 返回 feature-1 并合并 feature-1.1
git checkout feature-1
git merge feature-1.1 -m "feature-1: Merge feature-1.1 into feature-1"

# 返回主分支并添加不同的更新
git checkout master
echo "Another update in master branch" >> README.md
git add README.md
git commit -m "master: Another update in master branch"

# 创建第二个分支并做一些不同的更改
git checkout -b feature-2
echo "Feature 2 added" > feature2.txt
git add feature2.txt
git commit -m "feature-2: Add feature 2"

# 在 feature-2 上增加更多提交
echo "Feature 2 update 1" >> feature2.txt
git add feature2.txt
git commit -m "feature-2: Update feature 2 - part 1"

echo "Feature 2 update 2" >> feature2.txt
git add feature2.txt
git commit -m "feature-2: Update feature 2 - part 2"

# 创建一个新的分支 feature-2.1 从 feature-2
git checkout -b feature-2.1
echo "Feature 2.1 work" > feature2_1.txt
git add feature2_1.txt
git commit -m "feature-2.1: Work on feature 2.1"

# 返回 feature-2 并合并 feature-2.1
git checkout feature-2
git merge feature-2.1 -m "feature-2: Merge feature-2.1 into feature-2"

# 返回主分支并添加更多提交
git checkout master
echo "Final update in master branch" >> README.md
git add README.md
git commit -m "master: Final update in master branch"

# 再添加两个节点到主分支
echo "Another final update in master branch" >> README.md
git add README.md
git commit -m "master: Another final update in master branch"

echo "Yet another final update in master branch" >> README.md
git add README.md
git commit -m "master: Yet another final update in master branch"

# 进 rebase 操作，将 feature-2 rebase 到最新的 master
git checkout feature-2
git rebase master

# 合并 feature-1 到主分支
git checkout master
git merge feature-1 -m "master: Merge feature-1 into master"

# 合并 feature-2 到主分支
git merge feature-2 -m "master: Merge feature-2 into master"

# 展示提交历史
git log --graph --oneline --all

# 完成
echo "Demo repository with branches created, rebased, and merged successfully."

