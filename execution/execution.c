/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 07:47:27 by oscar             #+#    #+#             */
/*   Updated: 2026/03/23 10:44:15 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
#include "../built_ins/provisionaryFunctions.h"

/*static void	error_fork(t_exec_pack *p)
{
	perror("minishell: fork");
	change_exit_value("1", &p->envp);
	ft_free_malloc();
	free_envp_copy(p->envp);
	cmd_list_destroy(*p->data);
	free(p->line);
	intvec_destroy(p->codes);
	exit(1);
}*/

/*static void	error_pipe(t_exec_pack *p)
{
	perror("minishell: pipe");
	change_exit_value("1", &p->envp);
	free_envp_copy(p->envp);
	cmd_list_destroy(*p->data);
	free(p->line);
	intvec_destroy(p->codes);
	exit(1);
}*/

int	exec_exit_builtin(t_cmdnode *data_tmp, t_exec_pack *p)
{
	t_exit_ctx	ctx;

	ctx.args = data_tmp->phrase;
	ctx.in_child = 0;
	ctx.envp = &p->envp;
	ctx.data = p->data;
	ctx.line = p->line;
	ctx.codes = p->codes;
	return (builtin_exit(&ctx));
}

static void	handle_first_cmd(t_exec_pack *p, t_cmdnode *data_tmp)
{
	t_list_red	*tmp_infiles;
	t_list_red	*tmp_outfiles;

	tmp_infiles = data_tmp->infiles;
	if (tmp_infiles)
		handle_infile_redirections(&tmp_infiles, p);
	tmp_outfiles = data_tmp->outfiles;
	if (tmp_outfiles)
		handle_outfile_redirections(&tmp_outfiles, p);
	if (p->len > 1 && data_tmp->outfiles == NULL)
		dup2(p->arr_fds[0][1], STDOUT_FILENO);
}

static void	handle_last_cmd(t_exec_pack *p, t_cmdnode *data_tmp, int i)
{
	t_list_red	*tmp_infiles;
	t_list_red	*tmp_outfiles;

	tmp_infiles = data_tmp->infiles;
	if (tmp_infiles)
		handle_infile_redirections(&tmp_infiles, p);
	else
		dup2(p->arr_fds[i - 1][0], STDIN_FILENO);
	tmp_outfiles = data_tmp->outfiles;
	if (tmp_outfiles)
		handle_outfile_redirections(&tmp_outfiles, p);
}

static void	handle_middle_cmd(t_exec_pack *p, t_cmdnode *data_tmp, int i)
{
	t_list_red	*tmp_infiles;
	t_list_red	*tmp_outfiles;

	tmp_infiles = data_tmp->infiles;
	if (tmp_infiles)
		handle_infile_redirections(&tmp_infiles, p);
	else
		dup2(p->arr_fds[i - 1][0], STDIN_FILENO);
	tmp_outfiles = data_tmp->outfiles;
	if (tmp_outfiles)
		handle_outfile_redirections(&tmp_outfiles, p);
	else
		dup2(p->arr_fds[i][1], STDOUT_FILENO);
}

void	setup_child_redirections(t_exec_pack *p,
	t_cmdnode *data_tmp, int i)
{
	if (i == 0)
		handle_first_cmd(p, data_tmp);
	else if (i == p->len - 1)
		handle_last_cmd(p, data_tmp, i);
	else
		handle_middle_cmd(p, data_tmp, i);
}

char	**get_args_safe(t_cmdnode *data_tmp)
{
	if (data_tmp->cmd != NULL)
		return (get_args(data_tmp->phrase));
	return (NULL);
}

void	exec_external(char *cmd_path, char **args, char **envp)
{
	char	*error;

	if (cmd_path && args && execve(cmd_path, args, envp) < 0)
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

int	init_exec_pack(t_exec_pack *p)
{
	int	len;

	len = init_len_and_pipes(p);
	p->len = len;
	return (len);
}

int	process_exit_status(int last_exit_status)
{
	if (WIFEXITED(last_exit_status))
		return (WEXITSTATUS(last_exit_status));
	else if (WIFSIGNALED(last_exit_status))
		return (128 + WTERMSIG(last_exit_status));
	return (last_exit_status);
}

int	update_exit_value(int status, char ***envp)
{
	char	*status_str;

	if (g_sigint_received)
	{
		change_exit_value("130", envp);
		g_sigint_received = 0;
		return (130);
	}
	status_str = ft_itoa(status);
	change_exit_value(status_str, envp);
	return (status);
}

