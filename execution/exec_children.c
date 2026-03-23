/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_children.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 08:46:30 by oscar             #+#    #+#             */
/*   Updated: 2026/03/23 09:34:38 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	exec_child_command(t_exec_pack *p, t_cmdnode *data_tmp)
{
	char	**args;
	char	*cmd_path;

	if (is_built_in(data_tmp->cmd))
		return (exec_builtin_parent(data_tmp, p));
	args = get_args_safe(data_tmp);
	cmd_path = get_cmd_path_safe(p, data_tmp, args);
	exec_external(cmd_path, args, p->envp);
	return (0);
}

static void	child_cleanup_and_exit(t_exec_pack *p, int return_value)
{
	ft_free_malloc();
	free_envp_copy(p->envp);
	cmd_list_destroy(*p->data);
	free(p->line);
	intvec_destroy(p->codes);
	exit(return_value);
}

void	child_exec(t_exec_pack *p, t_cmdnode *data_tmp, int i)
{
	int	return_value;

	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	setup_child_redirections(p, data_tmp, i);
	close_fds(p->arr_fds, p->len);
	return_value = exec_child_command(p, data_tmp);
	child_cleanup_and_exit(p, return_value);
}
