/*************************************************************************
    > File Name: mlib.h
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Mon 05 May 2025 10:48:08 AM CST
 ************************************************************************/

#ifndef __MLIB_H__
#define __MLIB_H__

#ifdef __cplusplus
extern "C" {
#endif

// 初始化库
int lib_init();

// 执行工作
void lib_do_work(const char *prefix);

// 清理资源
void lib_cleanup();

#ifdef __cplusplus
}
#endif


#endif /* MLIB_H__ */
