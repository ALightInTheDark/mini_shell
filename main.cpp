#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "executer.h"
#include "global_variable.h"

void signal_handler(int signum)
{
	fprintf(stdout, "\n 收到%d号信号! 刷新输出缓冲区---", signum); fflush(stdout);
	fprintf(stdout, "\n mini shell: "); fflush(stdout);
}

void at_exit() { fprintf(stdout, "期待您的下次使用！\n"); }

int main()
{
	if (signal(SIGINT, signal_handler) == SIG_ERR) { perror("signal"); exit(EXIT_FAILURE); }
	if (signal(SIGQUIT, signal_handler) == SIG_ERR) { perror("signal"); exit(EXIT_FAILURE); }
	if (signal(SIGTTOU, SIG_IGN)== SIG_ERR) { perror("signal"); exit(EXIT_FAILURE); } // 见executer.cpp中44行注释

	if (atexit(at_exit) != 0) { fprintf(stderr, "atexit failed"); }

	while (true)
	{
		fprintf(stdout, "\033[32mmini shell: "); fflush(stdout); // \033[32m输出绿色文字

		reset_global_variables();

		if (!read_cmd()) { fprintf(stdout, "您输入了EOF，退出mini shell！\n"); break; }
		parse_cmd();
		execute_disk_command();
	}
}

/*
* ps
* ls -l -h
* ls -lh
* ls -lh | grep mini_shell | wc -w
* ps -aux|grep mini_shell
* ps -aux|grep bash | grep -v grep
* cat < test.txt | grep -n extern | wc -l > test2.txt (grep -n 显示行号)
* ls & | wc
*/