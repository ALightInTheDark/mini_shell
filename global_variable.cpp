// mini_shell
// Created by kiki on 2022/2/8.12:34
#include "global_variable.h"

char cmdline[MAXLINELENGTH + 1];
char arg_vector[MAXLINELENGTH + 1];
char* cmdline_ptr;
char* argv_ptr;

Command commands[MAXPIPELINE];

char input_file[MAXFILENAME + 1];
char output_file[MAXFILENAME + 1];

bool background_job;
int single_command_count;
bool append;

#include <string.h> // memset
void reset_global_variables()
{
	memset(commands, 0, sizeof(commands));
	for (int i = 0; i< MAXPIPELINE; ++i)
	{
		commands[i].input_fd = 0;
		commands[i].output_fd = 1;
	}

	memset(cmdline, 0, sizeof(cmdline));
	memset(arg_vector, 0, sizeof(arg_vector));
	cmdline_ptr = cmdline;
	argv_ptr = arg_vector;

	memset(input_file, 0, sizeof(input_file));
	memset(output_file, 0, sizeof(output_file));

	single_command_count = 0;
	background_job = false;
	append = false;
}