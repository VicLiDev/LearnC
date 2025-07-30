/*************************************************************************
    > File Name: main.c
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Wed 30 Jul 2025 09:48:15 AM CST
 ************************************************************************/

#include "blackbox.h"

int main()
{
    BlackBox box;
    blackbox_init(&box);

    blackbox_log(&box, 1001, "Memory allocation failed");
    blackbox_log(&box, 1002, "Sensor disconnected");
    blackbox_log(&box, 1003, "Unexpected reboot");

    blackbox_export(&box);  // 导出日志以查看

    // blackbox_clear(&box); // 如需清空

    return 0;
}

