/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtins.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 08:45:47 by oscar             #+#    #+#             */
/*   Updated: 2026/03/23 09:31:34 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	exec_simple_builtin(t_cmdnode *data_tmp, t_exec_pack *p)
{
	if (data_tmp && !ft_strncmp(data_tmp->cmd, "echo", 4))
		return (builtin_echo(data_tmp->phrase));
	else if (data_tmp && !ft_strncmp(data_tmp->cmd, "cd", 2))
		return (builtin_cd(data_tmp->phrase, p->envp));
	else if (data_tmp && !ft_strncmp(data_tmp->cmd, "pwd", 3))
		return (builtin_pwd());
	else if (data_tmp && !ft_strncmp(data_tmp->cmd, "export", 6))
		return (builtin_export(data_tmp->phrase, &p->envp));
	else if (data_tmp && !ft_strncmp(data_tmp->cmd, "unset", 5))
		return (builtin_unset(data_tmp->phrase, &p->envp));
	else if (data_tmp && !ft_strncmp(data_tmp->cmd, "env", 3))
		return (builtin_env(p->envp));
	return (-1);
}

int	exec_builtin_parent(t_cmdnode *data_tmp, t_exec_pack *p)
{
	int	return_value;

	return_value = exec_simple_builtin(data_tmp, p);
	if (return_value != -1)
		return (return_value);
	if (data_tmp && !ft_strncmp(data_tmp->cmd, "exit", 4))
		return (exec_exit_builtin(data_tmp, p));
	return (0);
}
