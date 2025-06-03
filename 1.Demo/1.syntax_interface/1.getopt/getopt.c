/*************************************************************************
    > File Name: getopt.c
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Tue 26 Nov 12:16:49 2024
 ************************************************************************/

/* usage: ./<exe> -v 12 -o moutput.txt */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>


typedef struct cmd_paras_t {
    int  ver;
    char *output_fname;
} cmdParas;

void print_usage(FILE* stream, int exit_code, const char *program_name)
{
    fprintf (stream, "Usage: %s options [ -o output_file ... ]\n", program_name);
    fprintf (stream,
             " -h Display this usage information.\n"
             " -v Version messages.\n"
             " -o Output filename.\n");
    exit (exit_code);
}

int proc_paras(int argc, char **argv, cmdParas *parse_paras)
{
    const char* program_name = argv[0];
    int opt;
    /* 单个字符a        表示选项a没有参数         格式：-a即可，不加参数 */
    /* 单字符加冒号b:   表示选项b有且必须加参数   格式：-b 100或-b100,但-b=100错 */
    /* 单字符加2冒号c:: 表示选项c可以有，也可以无 格式：-c200，其它格式错误 */
    char *optstr = "hv:o:";

    /*
     * getopt
     *
     * getopt用来解析命令行参数，但只能解析短选项: -d 100,不能解析长选项：--prefix
     *
     * 返回值：
     * getopt 返回匹配到的选项字符。如果选项需要参数，则参数值会保存在全局变量 optarg 中
     * 如果遇到无效选项或缺少参数，getopt 返回 '?'
     * 如果所有选项都被处理完，返回 -1
     */
    while ((opt = getopt(argc, argv, optstr))!= -1) {
        /*
         * optarg —— 指向当前选项参数(如果有)的指针
         * optind —— 再次调用 getopt() 时的下一个 argv指针的索引
         * optopt —— 最后一个未知选项
         * opterr —— 如果不希望getopt()打印出错信息，则只要将全域变量opterr设为0即可
         * printf("opt = %c\t\t", opt);
         * printf("optarg = %s\t\t",optarg);
         * printf("optind = %d\t\t",optind);
         * printf("argv[optind] = %s\n",argv[optind]);
         */
        switch(opt) {
            case 'h':
                print_usage (stdout, 0, program_name);
                break;
            case 'v':
                parse_paras->ver = atoi(optarg);
                break;
            case 'o':
                parse_paras->output_fname = optarg;
                break;
            case '?':
                print_usage (stdout, 0, program_name);
                break;
            default: /* 别的什么:非预料中的 */
                abort ();
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    cmdParas mcmd_paras = {0, NULL};
    proc_paras(argc, argv, &mcmd_paras);

    printf("version  No: %d\n", mcmd_paras.ver);
    printf("output file: %s\n",mcmd_paras.output_fname);
    return 0;
}
