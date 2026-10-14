/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 07:47:27 by oscar             #+#    #+#             */
/*   Updated: 2026/10/14 23:17:47 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	execute_pipeline(t_exec_pipe_ctx c)
{
	int	return_value;

	return_value = setup_and_redirect(&c);
	handle_exec_error(&c, return_value);
	return_value = execute_cmd_or_builtin(&c);
	clean_execution(c.p->envp, c.p->data, c.p->line, c.p->codes);
	exit(return_value);
}

int	init_exec(t_exec_init_ctx c)
{
	int	len;

	len = len_data(*c.data);
	*c.ctx = init_variable_ctx(c.envp, c.data, c.line, c.codes);
	if (len > 1)
		*c.arr_fds = execute_pipe(c.data, &c.codes, c.line, *c.envp);
	*c.p = init_variable_p((t_exec_args){c.envp, c.data, c.line,
			c.codes, *c.arr_fds, len});
	return (len);
}

void	exec_loop(t_cmdnode *data_tmp, t_exec_pack *p,
	t_exit_ctx ctx, pid_t *pids)
{
	int				i;
	t_exec_pipe_ctx	c;

	i = 0;
	while (i < p->len)
	{
		pids[i] = fork();
		if (pids[i] == -1)
			error_fork(p);
		else if (pids[i] == 0)
		{
			c.i = i;
			c.data_tmp = data_tmp;
			c.ctx = ctx;
			c.p = p;
			execute_pipeline(c);
		}
		i++;
		data_tmp = data_tmp->next;
	}
	close_fds(p->arr_fds, p->len);
}

int	execute_cmds(t_cmdnode **data, char ***envp,
	char *line, t_intvec *codes)
{
	t_exec_cmds_ctx	c;

	c.pids = NULL;
	c.arr_fds = NULL;
	c.last_exit_status = 0;
	c.len = init_exec((t_exec_init_ctx){
			data, envp, line, codes, &c.p, &c.ctx, &c.arr_fds});
	c.data_tmp = *data;
	if (c.len == 1
		&& (check_if_single_cmd_parent(c.data_tmp->cmd) == 1))
		return (handle_single_cmd_parent(c.data_tmp,
				data, envp, &c.ctx));
	c.pids = ft_malloc(sizeof(pid_t) * c.len);
	signal(SIGINT, handle_sigint);
	signal(SIGQUIT, SIG_IGN);
	exec_loop(c.data_tmp, &c.p, c.ctx, c.pids);
	c.last_exit_status = wait_and_signals(c.len, c.pids,
			c.last_exit_status, *envp);
	return (c.last_exit_status);
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
