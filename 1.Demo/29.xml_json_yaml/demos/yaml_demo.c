/*************************************************************************
    > File Name: yaml_demo.c
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Tue 27 May 2025 03:23:11 PM CST
 ************************************************************************/

#include <stdio.h>

int main()
{
    FILE *f = fopen("data.yaml", "w");
    if (f) {
        fprintf(f, "name: Alice\nage: 30\nskills:\n  - C\n  - YAML\n");
        fclose(f);
    }

    printf("YAML file created. Manual parsing or libfyaml recommended for full manipulation.\n");
    return 0;
}
