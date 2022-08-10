#include "microshell.h"

int	is_there_pipe(char **av)
{
	while (*av)
	{
		if (!strncmp(*av, "|", 2))
			return(*av = NULL, 1);
		if (!strncmp(*av, ";", 2))
			return(*av = NULL, 0);
		av++;
	}
	return (0);
}

void	init_struct(t_cmd *cmd, char **av)
{
	cmd->pid = 0;
	cmd->exit_status = 0;
	cmd->next = is_there_pipe(av);
	cmd->fd_next[0] = -1;
	cmd->fd_next[1] = -1;
}

int	ft_strlen(char *str)
{
	int	len;

	len = 0;
	while (str && str[len])
		len++;
	return (len);
}

int	ft_error(char *err_arg, int err_id, int ret)
{
	if (err_id == CD_ERROR_ID &&  ret == 1)
		write(2, CD_ERROR_MSG_1, ft_strlen(CD_ERROR_MSG_1));
	else if (err_id == CD_ERROR_ID && ret == 2)
	{
		write(2, CD_ERROR_MSG_2, ft_strlen(CD_ERROR_MSG_1));
		write(2, err_arg, ft_strlen(err_arg));
		write(2, "\n", 1);
	}
	else if(err_id == FATAL_ERROR_ID)
		write(2, FATAL_ERROR_MSG, ft_strlen(FATAL_ERROR_MSG));
	else if(err_id == EXEC_ERROR_ID)
	{
		write(2, EXEC_ERROR_MSG, ft_strlen(EXEC_ERROR_MSG));
		write(2, err_arg, ft_strlen(err_arg));
		write(2, "\n", 1);
	}
	return (ret);
}

void	ft_cd(char **av)
{
	if (!av[1])
		ft_error(NULL, CD_ERROR_ID, 1);
	else if (chdir(av[1]))
		ft_error(av[1], CD_ERROR_ID, 2);
}

int	exec_child(t_cmd *cmd, char **av, char **env)
{
	if (cmd->prev && dup2(cmd->fd_prev[0], 0) == -1)
		exit(-1);
	if (cmd->next && dup2(cmd->fd_next[1], 1) == -1)
		exit(-1);
	if (execve(av[0], av, env) == -1)
		return (1);
	return (0);
}

int	ft_wait(t_cmd *cmd)
{
	if (waitpid(-1, &cmd->exit_status, 0) == -1)
		return (-1);
	if (cmd->prev)
		close(cmd->fd_prev[0]); // you'll reaalize easily that since the function is called recursively all the fds open will end up being closed -- give it a thought and you'll see
	if (cmd->next)
		close(cmd->fd_next[1]);
	return (0);
}

int	exec(int ac, char **av, char **env, t_cmd *cmd)
{
	int	i;

	i = 0;
	if (ac < 1)
		return (0);
	init_struct(cmd, av);
	if (*av)
	{
		if (!strncmp(*av, "cd", 3))
			ft_cd(av);
		else
		{
			if (cmd->next && pipe(cmd->fd_next))
				return (ft_error(NULL, FATAL_ERROR_ID, 3));
			cmd->pid = fork();
			if (cmd->pid == -1)
				return (ft_error(NULL, FATAL_ERROR_ID, 4));
			if (!cmd->pid && exec_child(cmd, av, env))
				return (ft_error(av[0], EXEC_ERROR_ID, 5));
			if (ft_wait(cmd))
				return (ft_error(NULL, FATAL_ERROR_ID, 6));
		}
	}
	while (av[i])
		i++;
	cmd->prev = cmd->next;
	cmd->fd_prev[0] = cmd->fd_next[0];
	cmd->fd_prev[1] = cmd->fd_next[1];
	return (exec(ac - 1 - i, av + 1 + i, env, cmd));
}

int	main(int ac, char **av, char **env)
{
	t_cmd	cmd;

	cmd.prev = 0;
	cmd.fd_prev[0] = 0;
	cmd.fd_prev[1] = 1;
	return (exec(ac - 1, av + 1, env, &cmd));
}
