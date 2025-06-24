/*************************************************************************
    > File Name: test.cpp
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Tue 24 Jun 2025 08:22:16 PM CST
 ************************************************************************/

#include "MemoryPool.h"
#include "PoolAllocator.h"
#include <vector>
#include <string>
#include <unordered_map>

int main() {
    {
        using StrVec = std::vector<std::string, PoolAllocator<std::string>>;
        StrVec vec;
        for (int i = 0; i < 10; ++i)
            vec.emplace_back("Vector item #" + std::to_string(i));

        for (const auto& s : vec)
            std::cout << s << std::endl;
    }

    {
        using MyMap = std::unordered_map<int, std::string, std::hash<int>, std::equal_to<int>, PoolAllocator<std::pair<const int, std::string>>>;
        MyMap map;
        for (int i = 0; i < 10; ++i)
            map[i] = "MapVal#" + std::to_string(i);
    }

    // 内存池会在程序退出时析构，自动输出统计信息和泄漏情况
    return 0;
}

