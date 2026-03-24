/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   borrador.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 20:12:28 by oscar             #+#    #+#             */
/*   Updated: 2026/03/24 20:13:17 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	wait_and_signals(int len, pid_t *pids,
		int last_exit_status, char **envp)
{
	int	signal_code;
	int	i;

	signal_code = 0;
	i = 0;
	while (i < len)
	{
		waitpid(pids[i], &last_exit_status, 0);
		i++;
	}
	signal(SIGINT, handle_sigint);
	signal(SIGQUIT, SIG_IGN);
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
	change_exit_value(ft_itoa(last_exit_status), &envp);
	return (last_exit_status);
}

void	execute_execve(t_cmdnode *data_tmp, char **envp, t_exec_pack p)
{
	char	**args;
	char	*cmd_path;

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
		if (execve(cmd_path, args, envp) < 0)
			error_execve(envp, cmd_path);
	}
}

int	setup_and_redirect(t_exec_pipe_ctx *c)
{
	int	return_value;

	return_value = 0;
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	c->ctx.args = c->data_tmp->phrase;
	c->ctx.in_child = 1;
	if (c->i == 0)
		return_value = execute_red_first_cmd(c->data_tmp->infiles,
				c->data_tmp->outfiles, c->p->len, c->p->arr_fds);
	else if (c->i == c->p->len - 1)
		return_value = execute_red_last_cmd(c->data_tmp->infiles,
				c->data_tmp->outfiles, c->p->arr_fds, c->i);
	else
		return_value = execute_other_cmds(c->data_tmp->infiles,
				c->data_tmp->outfiles, c->p->arr_fds, c->i);
	close_fds(c->p->arr_fds, c->p->len);
	return (return_value);
}

void	handle_exec_error(t_exec_pipe_ctx *c, int return_value)
{
	if (return_value != 0 || !c->data_tmp->cmd)
	{
		clean_execution(c->p->envp, c->p->data,
			c->p->line, c->p->codes);
		exit(return_value);
	}
}

int	execute_cmd_or_builtin(t_exec_pipe_ctx *c)
{
	int	return_value;

	return_value = 0;
	if (is_built_in(c->data_tmp->cmd) == 1)
		return_value = execute_built_ins(c->data_tmp,
				&c->p->envp, &c->ctx);
	else
		execute_execve(c->data_tmp, c->p->envp, *c->p);
	return (return_value);
}
