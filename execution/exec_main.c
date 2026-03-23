/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_main.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 08:48:27 by oscar             #+#    #+#             */
/*   Updated: 2026/03/23 10:28:51 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	execute_cmds(t_cmdnode **data, char ***envp,
	char *line, t_intvec *codes)
{
	int			len;
	int			last_exit_status;
	t_cmdnode	*data_tmp;
	t_exec_pack	p;

	p.envp = *envp;
	p.data = data;
	p.line = line;
	p.codes = codes;
	len = init_exec_pack(&p);
	data_tmp = *data;
	if (len == 1 && (!data_tmp->cmd || data_tmp->cmd[0] == '\0'))
		return (handle_empty_case(envp));
	if (len == 1 && is_built_in(data_tmp->cmd))
		return (exec_builtin_parent(data_tmp, &p));
	last_exit_status = exec_forks(&p);
	last_exit_status = process_exit_status(last_exit_status);
	return (update_exit_value(last_exit_status, envp));
}
