/*************************************************************************
    > File Name: getopt_long.c
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Tue 26 Nov 11:44:19 2024
 ************************************************************************/

/* usage: ./<exe> --output o_file.txt -v hello */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>


typedef struct cmd_paras_t {
    int en_init;
    int en_clean;
    char *output;
    int  verbose; /* 是否显示冗余信息?*/
} cmd_paras;

void print_usage(FILE* stream, int exit_code, const char *program_name)
{
    fprintf (stream, "Usage: %s options [ -o|--output output_file ... ]\n", program_name);
    fprintf (stream,
             " -h|--help    Display this usage information.\n"
             " --init       Init.\n"
             " --clean      Clean.\n"
             " -o|--output  Write output to file.\n"
             " -v|--verbose Print verbose messages.\n");
    exit (exit_code);
}

int proc_paras(int argc, char* argv[], cmd_paras *paras)
{
    const char* program_name = argv[0];
    int opt;
    /*
     * 包含所有有效短选项字母的字符串
     * 单个字符a        表示选项a没有参数         格式：-a即可，不加参数
     * 单字符加冒号b:   表示选项b有且必须加参数   格式：-b 100或-b100,但-b=100错
     * 单字符加2冒号c:: 表示选项c可以有，也可以无 格式：-c200，其它格式错误
     */
    const char* const short_opts = "ho:v";
    /* 描述了长选项的 struct option 数组。*/
    /* *name;      长选项的名字（例如 "help"） */
    /* has_arg;    是否有附加参数： */
    /*               - no_argument (0): 无参数 */
    /*               - required_argument (1): 必须有参数 */
    /*               - optional_argument (2): 可选参数 */
    /* *flag;      如果不为 NULL，则设置该指针的值 */
    /* val;        如果 flag 为 NULL，则返回该值，便于后续case处理 */
    const struct option long_opts[] = {
        { "help",    no_argument,       NULL, 'h' },
        { "init",    no_argument,       NULL, 0   },  // --init（无短参数）
        { "clean",   no_argument,       NULL, 0   },  // --clean（无短参数）
        { "output",  required_argument, NULL, 'o' },
        { "verbose", required_argument, NULL, 'v' },
        { NULL, 0, NULL, 0 } /* 结束标志 */
    };
    int long_idx = 0;

    /*
     * getopt_long:
     * 如果成功解析一个选项，返回该选项对应的值。对于短选项，返回选项字符；对于长选项，返回 val 值。
     * 如果没有更多选项可以解析，返回 -1。
     * 如果遇到错误（例如无效的选项或缺少参数），返回 '?'。
     *
     * optarg 是一个全局变量，它在 getopt 和 getopt_long 函数解析命令行选项时，
     * 用来保存当前选项的参数值。如果选项需要一个参数，那么该参数的值会被存储在
     * optarg 中。
     */
    while ((opt = getopt_long(argc, argv, short_opts, long_opts, &long_idx)) != -1) {
        switch (opt) {
            case 'h': { /* -h 或 --help */
                print_usage (stdout, 0, program_name);
            } break;
            case 0: {
                // 处理无短参数的长选项
                const char* cur_opt_name = long_opts[long_idx].name;
                if (strcmp(cur_opt_name, "init") == 0)
                    paras->en_init = 1;
                else if (strcmp(cur_opt_name, "clean") == 0)
                    paras->en_clean = 1;
            } break;
            case 'o': { /* -o 或 --output */
                paras->output = optarg;
            }break;
            case 'v': { /* -v 或 --verbose */
                paras->verbose = 1;
            }break;
            case '?': {
                print_usage (stderr, 1, program_name);
            }break;
            default: { /* 别的什么:非预料中的 */
                abort ();
            }break;
        }
    }

    /* check paras */
    if (!paras->output) print_usage (stderr, 1, program_name);

    printf("======> cmd paras <======\n");
    printf("en_init:   %d\n", paras->en_init);
    printf("en_clean:  %d\n", paras->en_clean);
    printf("en_output: %s\n", paras->output);
    /* 选项处理完毕。OPTIND 指向第一个非选项参数。
     * 出于演示目的,如果指定了冗余输出选项,则输出这些参数。*/
    if (paras->verbose) {
        printf("verbose:   ");
        int i;
        for (i = optind; i < argc; ++i)
            printf ("%s ", argv[i]);
        printf("\n");
    } else {
        printf("verbose:   \n");
    }



    return 0;
}

int main(int argc, char *argv[])
{
    cmd_paras m_cmd_paras = {0};
    proc_paras(argc, argv, &m_cmd_paras);

    return 0;
}
