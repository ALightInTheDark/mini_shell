// mini_shell
// Created by kiki on 2022/2/8.16:26
#pragma once
#include "shell_def.h"

/*
* 假设用户输入的命令为cat < test.txt | wc -l >> test2.txt &
* 则cmdline为 cat < test.txt | wc -l >> test2.txt & \n\0 （&与\n\0间没有空格）
* 经过parse_cmd()函数处理后，arg_vector为 cat \0 wc \0 -l \0 （\0前没有空格）
* single_command_count为 2
* commands[0].args[0]指向cat,args[1]指向NULL; commands[1].args[0]指向wc, args[1]指向-l, args[2]指向NULL
* input_file为test.txt，output_file为test2.txt
*/
extern char cmdline[MAXLINELENGTH + 1];
extern char arg_vector[MAXLINELENGTH + 1];
extern char* cmdline_ptr;
extern char* argv_ptr;

extern Command commands[MAXPIPELINE];

extern char input_file[MAXFILENAME + 1];
extern char output_file[MAXFILENAME + 1];

extern bool background_job; // 是否为后台作业
extern int single_command_count; // 基本命令的个数
extern bool append; // 输出重定向是否为 >>

extern void reset_global_variables(); // 重置全局变量