/*************************************************************************
    > File Name: blackbox.h
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Wed 30 Jul 2025 09:47:17 AM CST
 ************************************************************************/

#ifndef __BLACKBOX_H__
#define __BLACKBOX_H__

#include <time.h>

#define MAX_LOG_COUNT 100
#define LOG_FILE_PATH "blackbox.log"

// 错误码定义
typedef enum {
    ERR_NONE = 0,
    ERR_MEMORY_ALLOCATION = 1001,
    ERR_SENSOR_DISCONNECTED = 1002,
    ERR_UNEXPECTED_REBOOT = 1003,
    ERR_FILE_IO_FAILURE = 1004,
    ERR_OVER_TEMPERATURE = 1005,
    // 可继续添加更多错误码...
} ErrorCode;

typedef struct {
    time_t timestamp;
    ErrorCode error_code;
    char description[256];
} ErrorLog;

typedef struct {
    int start;     // oldest log index, for loop ErrorLog
    int count;     // current number of logs
    ErrorLog logs[MAX_LOG_COUNT];
} BlackBox;

// 初始化黑盒系统（读取日志文件）
void blackbox_init(BlackBox* box);

// 添加一条异常记录
void blackbox_log(BlackBox* box, ErrorCode error_code, const char* description);

// 保存日志到文件
void blackbox_save_to_file(BlackBox* box);

// 加载日志从文件
void blackbox_load_from_file(BlackBox* box);

// 导出所有日志
void blackbox_export(const BlackBox* box);

// 清空日志
void blackbox_clear(BlackBox* box);

#endif /* BLACKBOX_H__ */
