// mini_shell
// Created by kiki on 2022/2/7.22:27
#include <stdio.h>
#include <unistd.h> // execvp, pipe, fork
#include "global_variable.h"
#include "executer.h"

/* 读取一条命令，读到EOF后返回false */
/* fgets 会读取'\n' 到字符数组中 */
bool read_cmd()
{
	if(!fgets(cmdline, MAXLINELENGTH, stdin)) { return false; }
	return true;
}

/* 匹配cmdline中的字符串与pattern字符串。
* 匹配成功返回true，并让cmdline_ptr跳过所匹配的字符串。*/
bool match_string(const char* pattern)
{
	while (*cmdline_ptr == ' ' || *cmdline_ptr == '\t') { ++cmdline_ptr; } // 跳过空白

	char* p = cmdline_ptr;
	while (*pattern != '\0' && *pattern == *p)
	{
		++pattern;
		++p;
	}

	if (*pattern == '\0') { cmdline_ptr = p; return true; }
	return false;
}

void get_filename(char* buf)
{
	while (*cmdline_ptr == ' ' || *cmdline_ptr == '\t') { ++cmdline_ptr; } // 跳过空白

	while (*cmdline_ptr != '\0' && *cmdline_ptr != '\n'
			&& *cmdline_ptr != ' ' && *cmdline_ptr != '\t'
			&& *cmdline_ptr != '<' && *cmdline_ptr != '>'
			&& *cmdline_ptr != '|' && *cmdline_ptr != '&')
		{ *buf++ = *cmdline_ptr++; } // 先 后++，再取内容
	*buf = 0;
}

/* 解析一条基本命令到commands[i].args中 */
void get_command(int i)
{
	int j = 0;
	while (*cmdline_ptr != '\0')
	{
		while (*cmdline_ptr == ' ' || *cmdline_ptr == '\t') { ++cmdline_ptr; } // 跳过空白

		commands[i].args[j] = argv_ptr;

		bool args_end;
		while (*cmdline_ptr != '\0' && *cmdline_ptr != '\n'
			   && *cmdline_ptr != ' ' && *cmdline_ptr != '\t'
			   && *cmdline_ptr != '<' && *cmdline_ptr != '>'
			   && *cmdline_ptr != '|' && *cmdline_ptr != '&')
		{
			*argv_ptr++ = *cmdline_ptr++;
			args_end = false;
		}
		*argv_ptr++ = '\0';

		switch (*cmdline_ptr)
		{
			case ' ': // 解析下一个参数
			case '\t':
				args_end = true;
				++j;
				break;
			case '<':
			case '>':
			case '|':
			case '&':
			case '\n':
				if (args_end) { commands[i].args[j] = NULL; } // 如果输入ls空格，没有这句代码，会把空格也解析为参数。因为53行代码；如果没有args_end条件，则会把ls命令解析为NULL
				return;
			default: // '\0'
				return;
		}
	}
}

void print_command()
{
	fprintf(stdout, "\033[34m解析到%d条基本命令 \n", single_command_count);

	if (input_file[0] != '\0') { fprintf(stdout, "输入重定向的文件路径是%s \n", input_file); }
	if (output_file[0] != '\0') { fprintf(stdout, "输出重定向的文件路径是%s \n", output_file); }


	for (int i=0; i < single_command_count; ++i)
	{
		fprintf(stdout, "第%d条基本命令的参数是 ", i);

		int j = 0;
		while (commands[i].args[j] != NULL)
		{
			fprintf(stdout, "[%s]", commands[i].args[j]);
			++j;
		}

		fprintf(stdout, "\n");
	}

	fprintf(stdout, "\033[30m\n");
}

/* 返回解析到的基本命令个数 */
void parse_cmd()
{
	if (match_string("\n")) { single_command_count = 0; return; } // 用户输入空命令
	if (exec_built_in_command()) { single_command_count = 0; return; } // 执行内部命令

	get_command(0); // 解析第一个基本命令
	if (match_string("<")) // 判断是否有输入重定向符
	{
		get_filename(input_file);
	}
	int i = 1; // 基本命令的个数
	for (; i < MAXPIPELINE; ++i) // 解析管道
	{
		if (match_string("|")) { get_command(i); }
		else { break; }
	}
	if (match_string(">")) // 判断是否有输出重定向符
	{
		if (match_string(">")) { append = true; }
		get_filename(output_file);
	}
	if (match_string("&")) // 判断是否有后台作业符
	{
		background_job = true;
	}
	if (match_string("\n")) // 命令是否正常结束
	{
		single_command_count = i;
		print_command();
	}
	else
	{
		single_command_count = -1;
		fprintf(stderr, "command line syntax error \n");
	}
}