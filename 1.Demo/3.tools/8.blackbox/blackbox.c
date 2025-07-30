/*************************************************************************
    > File Name: blackbox.c
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Wed 30 Jul 2025 09:47:42 AM CST
 ************************************************************************/

#include "blackbox.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void blackbox_init(BlackBox* box)
{
    box->start = 0;
    box->count = 0;
    blackbox_load_from_file(box);
}

void blackbox_log(BlackBox* box, ErrorCode error_code, const char* description)
{
    int index = (box->start + box->count) % MAX_LOG_COUNT;
    if (box->count == MAX_LOG_COUNT) {
        box->start = (box->start + 1) % MAX_LOG_COUNT;  // overwrite oldest
    } else {
        box->count++;
    }

    box->logs[index].timestamp = time(NULL);
    box->logs[index].error_code = error_code;
    strncpy(box->logs[index].description, description, sizeof(box->logs[index].description) - 1);
    box->logs[index].description[sizeof(box->logs[index].description) - 1] = '\0';

    blackbox_save_to_file(box);  // auto-save after logging
}

void blackbox_save_to_file(BlackBox* box)
{
    FILE* file = fopen(LOG_FILE_PATH, "wb");
    if (file) {
        fwrite(box, sizeof(BlackBox), 1, file);
        fclose(file);
    }
}

void blackbox_load_from_file(BlackBox* box)
{
    FILE* file = fopen(LOG_FILE_PATH, "rb");
    if (file) {
        fread(box, sizeof(BlackBox), 1, file);
        fclose(file);
    }
}

const char* error_code_to_string(ErrorCode code)
{
    switch (code) {
        case ERR_NONE:
            return "No Error";
        case ERR_MEMORY_ALLOCATION:
            return "Memory Allocation Failed";
        case ERR_SENSOR_DISCONNECTED:
            return "Sensor Disconnected";
        case ERR_UNEXPECTED_REBOOT:
            return "Unexpected Reboot";
        case ERR_FILE_IO_FAILURE:
            return "File I/O Failure";
        case ERR_OVER_TEMPERATURE:
            return "Over Temperature";
        default:
            return "Unknown Error";
    }
}

void blackbox_export(const BlackBox* box)
{
    printf("===== BlackBox Log Export =====\n");
    for (int i = 0; i < box->count; i++) {
        int index = (box->start + i) % MAX_LOG_COUNT;
        char timebuf[64];
        struct tm* tminfo = localtime(&box->logs[index].timestamp);
        strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", tminfo);

        printf("[%d] Time: %s | Code: %d (%s) | Desc: %s\n",
               i + 1, timebuf, box->logs[index].error_code,
               error_code_to_string(box->logs[index].error_code),
               box->logs[index].description);
    }
    printf("================================\n");
}

void blackbox_clear(BlackBox* box)
{
    box->start = 0;
    box->count = 0;
    remove(LOG_FILE_PATH);  // delete log file
}

