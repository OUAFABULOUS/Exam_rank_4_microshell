#ifndef MICROSHELL_H
# define MICROSHELL_H

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define CD_ERROR_ID 1
#define CD_ERROR_MSG_1 "error: cd: bad arguments\n"
#define CD_ERROR_MSG_2 "error: cd: cannot change directory "

#define FATAL_ERROR_ID 2
#define FATAL_ERROR_MSG "error: fatal\n"

#define EXEC_ERROR_ID 3
#define EXEC_ERROR_MSG "error: cannot execute "

typedef struct	s_cmd
{
	pid_t	pid;
	int		exit_status;
	int		prev;
	int		next;
	int		fd_prev[2];
	int		fd_next[2];
}				t_cmd;

#endif

