/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 07:47:27 by oscar             #+#    #+#             */
/*   Updated: 2026/03/24 13:07:08 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
#include "../built_ins/provisionaryFunctions.h"

static int	len_data(t_cmdnode *data)
{
	int	len;

	len = 0;
	while (data != NULL)
	{
		len++;
		data = data->next;
	}
	return(len);
}

static void	error_fork(t_exec_pack *p)
{
	perror("minishell: fork");
	change_exit_value("1", &p->envp);
	ft_free_malloc();
	free_envp_copy(p->envp);
	cmd_list_destroy(*p->data);
	free(p->line);
	intvec_destroy(p->codes);
	exit(1);
}

static void	error_pipe(t_cmdnode **data, t_intvec **codes, char *line, char **envp)
{
	perror("minishell: pipe");
	change_exit_value("1", &envp);
	free_envp_copy(envp);
	cmd_list_destroy(*data);
	free(line);
	intvec_destroy(*codes);
	exit(1);
}
/* 
static void	error_fd_file(char *name_infile, t_exec_pack *p)
{
	char	*error;

	error = ft_strjoin_modif("minishell: ", name_infile);
	perror(error);
	ft_free_malloc();
	change_exit_value("1", &p->envp);
	free_envp_copy(p->envp);
	cmd_list_destroy(*p->data);
	free(p->line);
	intvec_destroy(p->codes);
	exit(1);
} */

char	**get_args(t_list *phrase)
{
	t_list	*tmp;
	int		args_count;
	char	**args;
	int		i;

	args_count = 0;
	i = 0;
	tmp = phrase;
	while (tmp != NULL)
	{
		args_count++;
		tmp = tmp->next;
	}
	args = (char **)ft_malloc(sizeof(char *) * (args_count + 2));
	tmp = phrase;
	while (i < args_count)
	{
		args[i + 1] = tmp->content;
		tmp = tmp->next;
		i++;
	}
	args[args_count + 1] = NULL;
	return (args);
}

static void	exit_error(t_exec_pack *p, char *code, int exit_code)
{
	change_exit_value(code, &p->envp);
	free_envp_copy(p->envp);
	cmd_list_destroy(*p->data);
	free(p->line);
	intvec_destroy(p->codes);
	exit(exit_code);
}

static void	handle_fok_error(t_exec_pack *p, char *data_cmd)
{
	char	*error;

	error = ft_strjoin_modif("minishell: ", data_cmd);
	perror(error);
	close_fds(p->arr_fds, p->len);
	ft_free_malloc();
	if (errno == ENOTDIR)
		exit_error(p, "126", 126);
	else if (errno == ENOENT)
		exit_error(p, "127", 127);
}

static void	handle_xok_error(t_exec_pack *p, char *data_cmd)
{
	char	*error;

	error = ft_strjoin_modif("minishell: ", data_cmd);
	perror(error);
	change_exit_value("126", &p->envp);
	close_fds(p->arr_fds, p->len);
	free_envp_copy(p->envp);
	cmd_list_destroy(*p->data);
	free(p->line);
	intvec_destroy(p->codes);
	exit(126);
}

static char	*handle_direct_cmd(t_exec_pack *p, char *data_cmd)
{
	if (access(data_cmd, F_OK) != 0)
		handle_fok_error(p, data_cmd);
	if (access(data_cmd, X_OK) != 0)
		handle_xok_error(p, data_cmd);
	return (data_cmd);
}

static char	*get_cmd_name_and_path(t_exec_pack *p,
	char **args, char *data_cmd)
{
	char	*cmd_name;
	char	*cmd_path;

	cmd_path = NULL;
	if (find_chr(data_cmd, '/') == 1)
	{
		cmd_name = strchr_modif(data_cmd, '/');
		cmd_path = handle_direct_cmd(p, data_cmd);
		args[0] = cmd_name;
	}
	else
	{
		cmd_path = get_cmd_path(p, data_cmd);
		args[0] = data_cmd;
	}
	return (cmd_path);
}


static int	handle_infile_redirections(t_list_red **tmp_infiles)
{	
	int				fd_file;
	char			*error;
	
	while (*tmp_infiles != NULL)
	{
		if ((*tmp_infiles)->doble != 0)
		{
			dup2((*tmp_infiles)->doble, STDIN_FILENO);
			close((*tmp_infiles)->doble);
		}
		else
		{
			fd_file = open((*tmp_infiles)->content, O_RDONLY);
			if (fd_file == -1)
			{
				error = ft_strjoin_modif("minishell: ", (char *)(*tmp_infiles)->content);
				perror(error);
				return (1);
			}
			dup2(fd_file, STDIN_FILENO);
			close(fd_file);
		}
		*tmp_infiles = (*tmp_infiles)->next;
	}
	return (0);
}


static int	handle_outfile_redirections(t_list_red **tmp_outfiles)
{
	int	fd_file;
	char	*error;

	while (*tmp_outfiles != NULL)
	{
		if ((*tmp_outfiles)->doble == 0)
			fd_file = open((*tmp_outfiles)->content,
					O_WRONLY | O_CREAT | O_TRUNC, 0666);
		else
			fd_file = open((*tmp_outfiles)->content,
					O_WRONLY | O_CREAT | O_APPEND, 0666);
		if (fd_file == -1)
		{
			error = ft_strjoin_modif("minishell: ", (*tmp_outfiles)->content);
			perror(error);
			return(1);
		}
		dup2(fd_file, STDOUT_FILENO);
		close(fd_file);
		*tmp_outfiles = (*tmp_outfiles)->next;
	}
	return (0);
}

static int execute_built_ins(t_cmdnode *data_tmp, char **envp, t_exit_ctx ctx)
{
	int	return_value;

	return_value = 0;
	if (data_tmp->cmd && (ft_strncmp(data_tmp->cmd, "echo", 4) == 0 && data_tmp->cmd[4] == '\0'))
			return_value = builtin_echo(data_tmp->phrase);
	else if (data_tmp->cmd && (ft_strncmp(data_tmp->cmd, "cd", 2) == 0 && data_tmp->cmd[2] == '\0'))
		return_value = builtin_cd(data_tmp->phrase, envp);
	else if (data_tmp->cmd && (ft_strncmp(data_tmp->cmd, "pwd", 3) == 0 && data_tmp->cmd[3] == '\0'))
		return_value = builtin_pwd();
	else if (data_tmp->cmd && (ft_strncmp(data_tmp->cmd, "export", 6) == 0 && data_tmp->cmd[6] == '\0'))
		return_value = builtin_export(data_tmp->phrase, &envp);
	else if (data_tmp->cmd && (ft_strncmp(data_tmp->cmd, "unset", 5) == 0 && data_tmp->cmd[5] == '\0'))
		return_value = builtin_unset(data_tmp->phrase, &envp);
	else if (data_tmp->cmd && (ft_strncmp(data_tmp->cmd, "env", 3) == 0 && data_tmp->cmd[3] == '\0'))
		return_value = builtin_env(envp);
	else if (data_tmp->cmd && (ft_strncmp(data_tmp->cmd, "exit", 4) == 0 && data_tmp->cmd[4] == '\0'))
			return_value = builtin_exit(&ctx);
	return(return_value);
}

static int	**execute_pipe(t_cmdnode **data, t_intvec **codes, char *line, char **envp)
{
	int			**arr_fds;
	int			len;
	int			i;

	i = 0;
	len = len_data(*data);
	arr_fds = (int **)ft_malloc(sizeof(int *) * (len - 1));
	while (i < len - 1)
	{
		arr_fds[i] = (int *)ft_malloc(sizeof(int) * 2);
		if (pipe(arr_fds[i]) == -1)
			error_pipe(data, codes, line, envp);
		i++;
	}
	return (arr_fds);
}
static void error_execve(char **envp, char *cmd_path)
{
	char *error;

	error = ft_strjoin_modif("minishell: ", cmd_path);
	perror(error);
	if (errno == EACCES)
	{
		change_exit_value("126", &envp);
		exit(126);
	}
	else if (errno == ENOENT)
	{
		change_exit_value("127", &envp);
		exit(127);
	}
	else
	{
		change_exit_value("126", &envp);
		exit(126);
	}
}

static int	execute_only_redirect(t_list_red *data_infiles, t_list_red *data_outfiles)
{
	int	saved_stdin;
	int	saved_stdout;
	int	return_value;
	
	return_value = 0;
	saved_stdin = -1;
	saved_stdout = -1;
	saved_stdin = dup(STDIN_FILENO);
	if (data_infiles != NULL)
		return_value = handle_infile_redirections(&data_infiles);
	if (data_outfiles != NULL)
	{
		saved_stdout = dup(STDOUT_FILENO);
		return_value = handle_outfile_redirections(&data_outfiles);
	}
	if (saved_stdin != -1)
	{
		dup2(saved_stdin, STDIN_FILENO);
		close(saved_stdin);
	}
	if (saved_stdout != -1)
	{
		dup2(saved_stdout, STDOUT_FILENO);
		close(saved_stdout);
	}
	return (return_value);
}

int	execute_cmds(t_cmdnode **data, char ***envp, char *line, t_intvec *codes)
{
	pid_t		*pids;
	char		**args;
	char		*cmd_path;
	int			len;
	int			i;
	int			last_exit_status;
	int			**arr_fds;
	t_cmdnode	*data_tmp;
	int			saved_stdin;
	int			saved_stdout;
	t_list_red	*tmp_infiles;
	t_list_red	*tmp_outfiles;
	int			return_value;
	t_exit_ctx	ctx;
	int			signal_code;
	char		*status_str;
	t_exec_pack	p;

	len = 0;
	i = 0;
	last_exit_status = 0;
	data_tmp = *data;
	pids = NULL;
	arr_fds = NULL;
	return_value = 0;

	// For built_in exit!!!!!!
	ctx.args = data_tmp->phrase;
	ctx.in_child = 0;
	ctx.envp = envp;
	ctx.data = data;
	ctx.line = line;
	ctx.codes = codes;
	
	len = len_data(*data);
	if (len > 1) /// 1 comando
		arr_fds = execute_pipe(data, &codes, line, *envp);
	
	// for errors:
	p.envp = *envp;
	p.data = data;
	p.line = line;
	p.codes = codes;
	p.arr_fds = arr_fds;
	p.len = len;

	i = 0;
	data_tmp = *data;
	if (len == 1 && (data_tmp->cmd == NULL || data_tmp->cmd[0] == '\0')) // solo redirecciones
		return(execute_only_redirect((*data)->infiles, (*data)->outfiles));

	if (len == 1 && (is_built_in(data_tmp->cmd) == 1)) // solo 1 built_in
	{
		saved_stdin = -1;
		saved_stdin = dup(STDIN_FILENO);
		tmp_infiles = data_tmp->infiles;
		if (tmp_infiles != NULL)
		{
			if (tmp_infiles != NULL)
					return_value = handle_infile_redirections(&tmp_infiles);
		}
		saved_stdout = -1;
		tmp_outfiles = data_tmp->outfiles;
		if (tmp_outfiles != NULL)
		{
			saved_stdout = dup(STDOUT_FILENO);
			return_value = handle_outfile_redirections(&tmp_outfiles);
		}
		if (is_built_in(data_tmp->cmd) == 1)
			return_value = execute_built_ins(data_tmp, *envp, ctx);
		if (saved_stdin != -1)
		{
			dup2(saved_stdin, STDIN_FILENO);
			close(saved_stdin);
		}
		if (saved_stdout != -1)
		{
			dup2(saved_stdout, STDOUT_FILENO);
			close(saved_stdout);
		}
		return (return_value);
	}
	else
	{
		pids = ft_malloc(sizeof(pid_t) * len);
		signal(SIGINT, handle_sigint);
		signal(SIGQUIT, SIG_IGN);
		while (i < len)
		{
			pids[i] = fork();
			if (pids[i] == -1)
				error_fork(&p);
			else if (pids[i] == 0)
			{
				signal(SIGINT, SIG_DFL);
				signal(SIGQUIT, SIG_DFL);

				if (i == 0)
				{
					tmp_infiles = data_tmp->infiles;
					if (tmp_infiles != NULL)
						return_value = handle_infile_redirections(&tmp_infiles);
					tmp_outfiles = data_tmp->outfiles;
					if (tmp_outfiles != NULL)
						return_value = handle_outfile_redirections(&tmp_outfiles);
					if (len > 1 && data_tmp->outfiles == NULL)
						dup2(arr_fds[0][1], STDOUT_FILENO);
				}
				else if (i == len - 1)
				{
					tmp_infiles = data_tmp->infiles;
					if (tmp_infiles != NULL)
						return_value = handle_infile_redirections(&tmp_infiles);
					else
						dup2(arr_fds[i - 1][0], STDIN_FILENO);
					tmp_outfiles = data_tmp->outfiles;
					if (tmp_outfiles != NULL)
						return_value = handle_outfile_redirections(&tmp_outfiles);
				}
				else
				{
					tmp_infiles = data_tmp->infiles;
					if (tmp_infiles != NULL)
						return_value = handle_infile_redirections(&tmp_infiles);
					else
						dup2(arr_fds[i - 1][0], STDIN_FILENO);

					tmp_outfiles = data_tmp->outfiles;
					if (tmp_outfiles != NULL)
						return_value = handle_outfile_redirections(&tmp_outfiles);
					else
						dup2(arr_fds[i][1], STDOUT_FILENO);
				}

				close_fds(arr_fds, len);
				
				
				/////////////////////////////////////////
				if (return_value != 0)
				{
					ft_free_malloc();
					free_envp_copy(*envp);
					cmd_list_destroy(*data);
					free(line);
					intvec_destroy(codes);
					exit(return_value);
				}
				
				if (!data_tmp->cmd)
				{
					ft_free_malloc();
					free_envp_copy(*envp);
					cmd_list_destroy(*data);
					free(line);
					intvec_destroy(codes);
					exit(return_value);
				}
								
				if (is_built_in(data_tmp->cmd) == 1)
					return_value = execute_built_ins(data_tmp, *envp, ctx);
				else
				{
					if (data_tmp->cmd != NULL)
						args = get_args(data_tmp->phrase);
					else
						args = NULL;

					if (data_tmp->cmd != NULL)
						cmd_path = get_cmd_name_and_path(&p, args, data_tmp->cmd);
					else
						cmd_path = NULL;

					if (cmd_path && args)
					{
						if (execve(cmd_path, args, *envp) < 0)
							error_execve(*envp, cmd_path);
					}
				}
				ft_free_malloc();
				free_envp_copy(*envp);
				cmd_list_destroy(*data);
				free(line);
				intvec_destroy(codes);
				exit(return_value);
			}
			i++;
			data_tmp = data_tmp->next;
		}
		close_fds(arr_fds, len);

		i = 0;
		while (i < len)
		{
			waitpid(pids[i], &last_exit_status, 0);
			i++;
		}
		signal(SIGINT, handle_sigint);
		signal(SIGQUIT, SIG_IGN);
	}

	signal_code = 0;
	if (WIFEXITED(last_exit_status))
		last_exit_status = WEXITSTATUS(last_exit_status);
	else if (WIFSIGNALED(last_exit_status))
	{
		signal_code = WTERMSIG(last_exit_status);
		if (signal_code == SIGINT)
			write(1, "", 0);
		else if (signal_code == SIGQUIT)
			write(1, "Quit (core dumped)\n", 20);
		last_exit_status = signal_code + 128;
	}
	status_str = ft_itoa(last_exit_status);
	change_exit_value(status_str, envp);
	return (last_exit_status);
}
