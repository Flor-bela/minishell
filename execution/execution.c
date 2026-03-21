/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 07:47:27 by oscar             #+#    #+#             */
/*   Updated: 2026/03/21 12:50:43 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
#include "../built_ins/provisionaryFunctions.h"

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

static void	error_pipe(t_exec_pack *p)
{
	perror("minishell: pipe");
	change_exit_value("1", &p->envp);
	free_envp_copy(p->envp);
	cmd_list_destroy(*p->data);
	free(p->line);
	intvec_destroy(p->codes);
	exit(1);
}

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
}

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

static void	handle_heredoc_error(t_exec_pack *p)
{
	change_exit_value("130", &p->envp);
	ft_free_malloc();
	free_envp_copy(p->envp);
	cmd_list_destroy(*p->data);
	free(p->line);
	intvec_destroy(p->codes);
	exit(130);
}

static void	exec_heredoc(t_list_red *redir, t_exec_pack *p)
{
	t_heredoc_fork	ctx;
	char			*content;
	int				pipefd[2];

	ctx.delimiter = redir->content;
	ctx.expand = 1;
	ctx.env = p->envp;
	ctx.data = p->data;
	ctx.line = p->line;
	ctx.codes = p->codes;
	content = heredoc_with_fork(&ctx);
	if (!content)
		handle_heredoc_error(p);
	pipe(pipefd);
	write(pipefd[1], content, ft_strlen(content));
	close(pipefd[1]);
	dup2(pipefd[0], STDIN_FILENO);
	close(pipefd[0]);
	free(content);
}

static void	exec_infile(t_list_red *redir, t_exec_pack *p)
{
	int	fd_file;

	fd_file = open(redir->content, O_RDONLY);
	if (fd_file == -1)
	{
		close_fds(p->arr_fds, p->len);
		error_fd_file(redir->content, p);
	}
	dup2(fd_file, STDIN_FILENO);
	close(fd_file);
}

static void	handle_infile_redirections(t_list_red **tmp_infiles,
	t_exec_pack *p)
{
	while (*tmp_infiles != NULL)
	{
		if ((*tmp_infiles)->doble == 1)
			exec_heredoc(*tmp_infiles, p);
		else
			exec_infile(*tmp_infiles, p);
		*tmp_infiles = (*tmp_infiles)->next;
	}
}

static void	handle_outfile_redirections(t_list_red **tmp_outfiles,
	t_exec_pack *p)
{
	int	fd_file;

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
			close_fds(p->arr_fds, p->len);
			error_fd_file((*tmp_outfiles)->content, p);
		}
		dup2(fd_file, STDOUT_FILENO);
		close(fd_file);
		*tmp_outfiles = (*tmp_outfiles)->next;
	}
}

static int	init_len_and_pipes(t_exec_pack *p)
{
	int			i;
	t_cmdnode	*tmp;

	p->len = 0;
	i = 0;
	tmp = *p->data;
	while (tmp != NULL)
	{
		p->len++;
		tmp = tmp->next;
	}
	p->arr_fds = NULL;
	if (p->len > 1)
	{
		p->arr_fds = (int **)ft_malloc(sizeof(int *) * (p->len - 1));
		while (i < p->len - 1)
		{
			p->arr_fds[i] = (int *)ft_malloc(sizeof(int) * 2);
			if (pipe(p->arr_fds[i]) == -1)
				error_pipe(p);
			i++;
		}
	}
	return (p->len);
}

static int	handle_empty_case(t_cmdnode *data_tmp, char ***envp)
{
	change_exit_value("0", envp);
	return (0);
}

static int	exec_builtin_parent(t_cmdnode *data_tmp,
	char ***envp, char *line, t_intvec *codes, t_cmdnode **data)
{
	t_exit_ctx	ctx;
	int			return_value;

	return_value = 0;
	if (data_tmp && !ft_strncmp(data_tmp->cmd, "echo", 4))
		return_value = builtin_echo(data_tmp->phrase);
	else if (data_tmp && !ft_strncmp(data_tmp->cmd, "cd", 2))
		return_value = builtin_cd(data_tmp->phrase, *envp);
	else if (data_tmp && !ft_strncmp(data_tmp->cmd, "pwd", 3))
		return_value = builtin_pwd();
	else if (data_tmp && !ft_strncmp(data_tmp->cmd, "export", 6))
		return_value = builtin_export(data_tmp->phrase, envp);
	else if (data_tmp && !ft_strncmp(data_tmp->cmd, "unset", 5))
		return_value = builtin_unset(data_tmp->phrase, envp);
	else if (data_tmp && !ft_strncmp(data_tmp->cmd, "env", 3))
		return_value = builtin_env(*envp);
	else if (data_tmp && !ft_strncmp(data_tmp->cmd, "exit", 4))
	{
		ctx.args = data_tmp->phrase;
		ctx.in_child = 0;
		ctx.envp = envp;
		ctx.data = data;
		ctx.line = line;
		ctx.codes = codes;
		return_value = builtin_exit(&ctx);
	}
	return (return_value);
}

static void	child_exec(t_exec_pack *p, t_cmdnode *data_tmp,
	int i, char ***envp, int len, int **arr_fds,
	char *line, t_intvec *codes)
{
	char		**args;
	char		*cmd_path;
	char		*error;
	int			return_value;
	t_exit_ctx	ctx;
	t_list_red	*tmp_infiles;
	t_list_red	*tmp_outfiles;

	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);

	if (i == 0)
	{
		tmp_infiles = data_tmp->infiles;
		if (tmp_infiles)
			handle_infile_redirections(&tmp_infiles, p);
		tmp_outfiles = data_tmp->outfiles;
		if (tmp_outfiles)
			handle_outfile_redirections(&tmp_outfiles, p);
		if (len > 1 && data_tmp->outfiles == NULL)
			dup2(arr_fds[0][1], STDOUT_FILENO);
	}
	else if (i == len - 1)
	{
		tmp_infiles = data_tmp->infiles;
		if (tmp_infiles)
			handle_infile_redirections(&tmp_infiles, p);
		else
			dup2(arr_fds[i - 1][0], STDIN_FILENO);
		tmp_outfiles = data_tmp->outfiles;
		if (tmp_outfiles)
			handle_outfile_redirections(&tmp_outfiles, p);
	}
	else
	{
		tmp_infiles = data_tmp->infiles;
		if (tmp_infiles)
			handle_infile_redirections(&tmp_infiles, p);
		else
			dup2(arr_fds[i - 1][0], STDIN_FILENO);
		tmp_outfiles = data_tmp->outfiles;
		if (tmp_outfiles)
			handle_outfile_redirections(&tmp_outfiles, p);
		else
			dup2(arr_fds[i][1], STDOUT_FILENO);
	}

	close_fds(arr_fds, len);

	if (is_built_in(data_tmp->cmd))
	{
		return_value = exec_builtin_parent(data_tmp, envp, line, codes, p->data);
	}
	else
	{
		args = data_tmp->cmd ? get_args(data_tmp->phrase) : NULL;
		cmd_path = data_tmp->cmd ?
			get_cmd_name_and_path(p, args, data_tmp->cmd) : NULL;

		if (cmd_path && args && execve(cmd_path, args, *envp) < 0)
		{
			error = ft_strjoin_modif("minishell: ", cmd_path);
			perror(error);
			if (errno == EACCES)
				exit(126);
			else if (errno == ENOENT)
				exit(127);
			else
				exit(126);
		}
	}
	ft_free_malloc();
	free_envp_copy(*envp);
	cmd_list_destroy(*p->data);
	free(line);
	intvec_destroy(codes);
	exit(return_value);
}

static int	exec_forks(t_exec_pack *p, char ***envp,
	char *line, t_intvec *codes)
{
	pid_t		*pids;
	int			i;
	int			status;
	t_cmdnode	*tmp;

	pids = ft_malloc(sizeof(pid_t) * p->len);
	tmp = *p->data;
	i = 0;
	signal(SIGINT, handle_sigint);
	signal(SIGQUIT, SIG_IGN);

	while (i < p->len)
	{
		pids[i] = fork();
		if (pids[i] == 0)
			child_exec(p, tmp, i, envp, p->len,
				p->arr_fds, line, codes);
		tmp = tmp->next;
		i++;
	}
	close_fds(p->arr_fds, p->len);
	i = 0;
	while (i < p->len)
		waitpid(pids[i++], &status, 0);
	return (status);
}

int	execute_cmds(t_cmdnode **data, char ***envp,
	char *line, t_intvec *codes)
{
	int			len;
	int			last_exit_status;
	int			**arr_fds;
	t_cmdnode	*data_tmp;
	t_exec_pack	p;
	int			signal_code;
	char		*status_str;

	p.envp = *envp;
	p.data = data;
	p.line = line;
	p.codes = codes;

	len = init_len_and_pipes(&p);
	arr_fds = p.arr_fds;

	p.envp = *envp;
	p.data = data;
	p.line = line;
	p.codes = codes;
	p.arr_fds = arr_fds;
	p.len = len;

	data_tmp = *data;

	if (len == 1 && (!data_tmp->cmd || data_tmp->cmd[0] == '\0'))
		return (handle_empty_case(data_tmp, envp));

	if (len == 1 && is_built_in(data_tmp->cmd))
		return (exec_builtin_parent(data_tmp, envp, line, codes, data));

	last_exit_status = exec_forks(&p, envp, line, codes);

	signal_code = 0;
	if (WIFEXITED(last_exit_status))
		last_exit_status = WEXITSTATUS(last_exit_status);
	else if (WIFSIGNALED(last_exit_status))
		last_exit_status = 128 + WTERMSIG(last_exit_status);

	if (g_sigint_received)
	{
		change_exit_value("130", envp);
		g_sigint_received = 0;
		return (130);
	}

	status_str = ft_itoa(last_exit_status);
	change_exit_value(status_str, envp);
	return (last_exit_status);
}