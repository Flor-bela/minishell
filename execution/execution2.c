/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution2.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 20:04:09 by oscar             #+#    #+#             */
/*   Updated: 2026/03/24 20:05:59 by oscar            ###   ########.fr       */
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

void	error_fork(t_exec_pack *p)
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
