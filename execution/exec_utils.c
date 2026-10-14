/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 23:00:54 by fda-roch          #+#    #+#             */
/*   Updated: 2026/10/14 23:36:17 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	len_data(t_cmdnode *data)
{
	int	len;

	len = 0;
	while (data != NULL)
	{
		len++;
		data = data->next;
	}
	return (len);
}

t_exit_ctx	init_variable_ctx(char ***envp,
		t_cmdnode **data, char *line, t_intvec *codes)
{
	t_exit_ctx	ctx;

	ctx.args = (*data)->phrase;
	ctx.in_child = 0;
	ctx.envp = envp;
	ctx.data = data;
	ctx.line = line;
	ctx.codes = codes;
	return (ctx);
}

t_exec_pack	init_variable_p(t_exec_args a)
{
	t_exec_pack	p;

	p.envp = *a.envp;
	p.data = a.data;
	p.line = a.line;
	p.codes = a.codes;
	p.arr_fds = a.arr_fds;
	p.len = a.len;
	return (p);
}

void	clean_execution(char **envp,
		t_cmdnode **data, char *line, t_intvec *codes)
{
	ft_free_malloc();
	free_envp_copy(envp);
	cmd_list_destroy(*data);
	free(line);
	intvec_destroy(codes);
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
