/*************************************************************************
    > File Name: mlib.h
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Mon 05 May 2025 10:48:08 AM CST
 ************************************************************************/

#ifndef __MLIBCPP_H__
#define __MLIBCPP_H__

#include <stdio.h>
#include <unistd.h>

class TestLib {

public:
    TestLib()
    {
        printf("lib C++ initialized\n");
    }

    static TestLib* get_instance()
    {
        static TestLib ins;

        return &ins;
    }

    ~TestLib()
    {
        printf("lib C++ cleanup\n");
    }

    void work(const char *prefix);
};

#endif /* MLIBCPP_H__ */
