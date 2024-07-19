/*************************************************************************
    > File Name: asan_demo.cpp
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Tue 14 May 2024 03:13:53 PM CST
 ************************************************************************/

#include <iostream>
#include <vector>

int main()
{
    std::vector<int> vec = {1, 2, 3};

    // 故意越界访问
    std::cout << "Out-of-bounds access index 3: " << vec[3] << std::endl;

    // 创建一个野指针
    int *ptr = new int[3];
    delete[] ptr;
    // 访问野指针
    std::cout << "Accessing wild pointers: " << *ptr << std::endl;

    return 0;
}
