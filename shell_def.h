// mini_shell
// Created by kiki on 2022/2/8.16:30
// 该头文件只被global_variable.h包含
#pragma once

#define MAXLINELENGTH 1024	/* 输入行的最大长度 */
#define MAXPIPELINE 5		/* 一个管道行中简单命令的最多个数 */
#define MAXARG 20		/* 每个简单命令的参数最多个数 */
#define MAXFILENAME 100		/* IO重定向文件名的最大长度 */

struct Command
{
	char* args[MAXARG]; // 命令的参数列表
	int input_fd; // 输入重定向的文件描述符
	int output_fd; // 输出重定向的文件描述符
};