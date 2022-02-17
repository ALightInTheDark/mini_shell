// mini_shell
// Created by kiki on 2022/2/8.12:24
// fixme:open, signal, dup的错误处理
#include <stdio.h> // perror
#include <stdlib.h> // exit
#include <unistd.h> // execvp, pipe, fork, setpgid
#include <sys/wait.h> // wait
#include <fcntl.h> // open
#include "global_variable.h"

void fork_then_exec(int i) // 执行第i条基本命令
{
	pid_t pid = fork();
	if (pid == -1) { perror("fork failed "); exit(EXIT_FAILURE); }

	if (pid == 0) // 子进程，将会被命令程序替换
	{
		// 后台作业可以通过bg命令切换到前台，从标准输入获取数据。
		// 我们的mini shell暂不支持后台作业控制，因此直接返回EOF
		if (background_job && commands[i].input_fd == 0)
		{
			commands[i].input_fd = open("/dev/null", O_RDONLY);
		}

		if (commands[i].input_fd != 0) { close(0); dup(commands[i].input_fd); } // 将标准输入重定向到pipefd[0] 等价于dup2(commands[i].input_fd, 0);
		if (commands[i].output_fd != 1) { close(1); dup(commands[i].output_fd); } // 将标准输出重定向到pipefd[1]

		for (int j = 3; j < 1024; ++j) { close(j); } // 关闭可能继承自父进程的无用文件描述符

		int ret = setpgid(0, 0);  // 将子进程id设置为子进程进程组id，等价于setpgrp(); 可以用ps -xj命令查看。
		if (ret == -1) { perror("setpgid"); exit(EXIT_FAILURE); }

		execvp(commands[i].args[0], commands[i].args);

		perror("您输入的命令不正确"); _exit(-1); // 进程映像替换成功则不会执行到此
	}
	else // 父进程
	{
		if (background_job)
		{
			fprintf(stdout, "后台作业的pid是:%d \n", pid);
		}
		else
		{
			pid_t old = tcgetpgrp(0); // 获取前台进程组id
			tcsetpgrp(0,pid); // 让子进程所在的进程组获得终端，能够收到SIGINT和SIGQUIT信号以退出前台作业（子进程子进程的进程映像被完全替换，故其信号处理函数恢复为SIG_DFL）。父进程不能收到这两个信号。
			wait(NULL); // 父进程等待子进程退出
			tcsetpgrp(0, old); // 让父进程获得终端。父进程（后台进程）调用tcsetpgrp时，会收到SIGTTOU信号，需要忽略该信号
		}
	}
}

/*如果让当前进程调用execvp()执行用户命令，则整个进程映像被替换，执行完用户命令后，程序会直接退出
因此，我们让fork()出的子进程执行用户命令*/
void execute_disk_command()
{
	if (single_command_count <= 0) { return; } // 用户只输入回车, 或者用户执行了内建命令, 或者用户输入错误的命令

	if (input_file[0] != '\0') // 只有第一条基本命令能够有输入重定向
	{
		commands[0].input_fd = open(input_file, O_RDONLY);
	}
	if (output_file[0] != '\0') // 只有最后一条基本命令能够有输出重定向
	{
		if (append) { commands[single_command_count-1].output_fd = open(output_file, O_WRONLY | O_CREAT | O_APPEND, 0666); }
		else { commands[single_command_count-1].output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0666); }
	}

	if (background_job) { signal(SIGCHLD, SIG_IGN); } // 后台作业不会调用wait等待子进程退出，忽略SIGCHLD信号以避免僵尸进程
	else { signal(SIGCHLD, SIG_DFL); }

	int pipefd[2];
	for (int i = 0; i < single_command_count; ++i) // 命令示例：ls -lh | grep mini_shell | wc -w
	{
		if (i < single_command_count - 1) // 除了最后一条基本命令外，其余命令要创建管道
		{
			if (pipe(pipefd) == -1) { perror("创建管道失败 "); exit(EXIT_FAILURE); }

			// 当前命令从上一次创建的pipefd[0]中获取输入
			commands[i].output_fd = pipefd[1]; // 当前命令的输出写入到新创建的pipefd[1]
			commands[i + 1].input_fd =  pipefd[0]; // 下一条命令从新创建的pipefd[0]获取输入
		}

		fork_then_exec(i);

		if (commands[i].input_fd != 0) { close(commands[i].input_fd); } // 关闭上一次创建的pipefd[0]
		if (commands[i].output_fd != 1) { close(commands[i].output_fd); } // 关闭本次创建的pipefd[1]
	}
}



void exit()
{
	fprintf(stdout, "您输入了exit内建命令，退出mini shell! \n");
	exit(EXIT_SUCCESS);
}

using func_ptr = void(*)();
struct BuiltinCmd
{
	const char* name;
	func_ptr fp;
};

BuiltinCmd built_in_commands[] =
{
	{"exit", exit }
};

extern bool match_string(const char* pattern);
bool exec_built_in_command()
{
	for (auto& built_in_command : built_in_commands)
	{
		if (match_string(built_in_command.name))
		{
			built_in_command.fp();
			return true;
		}
	}
	return false;
}
