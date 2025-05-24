/*************************************************************************
    > File Name: iterator.cpp
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Sat 24 May 2025 08:56:26 AM CST
 ************************************************************************/

#include <iostream>
#include <vector>

// 自定义迭代器示例
class Range {
    int m_start, m_end;  // 改为使用 m_ 前缀避免命名冲突
public:
    Range(int s, int e) : m_start(s), m_end(e) {}

    // 迭代器类
    class Iterator {
        int current;
    public:
        Iterator(int c) : current(c) {}
        int operator*() { return current; }
        Iterator& operator++() { ++current; return *this; }
        bool operator!=(const Iterator& other) { return current != other.current; }
    };

    Iterator begin() { return Iterator(m_start); }
    Iterator end() { return Iterator(m_end + 1); }  // 修改 end() 返回 m_end + 1 以便包含最后一个元素
};

int main()
{
    // 使用STL迭代器
    std::vector<int> vec = {1, 2, 3, 4, 5};
    for (auto it = vec.begin(); it != vec.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;  // 输出: 1 2 3 4 5

    // 使用自定义迭代器
    for (int i : Range(10, 15)) {
        std::cout << i << " ";
    }
    // 输出: 10 11 12 13 14 15

    return 0;
}
