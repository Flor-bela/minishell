/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 23:10:39 by fda-roch          #+#    #+#             */
/*   Updated: 2026/10/14 23:30:25 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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

char	*handle_direct_cmd(t_exec_pack *p, char *data_cmd)
{
	if (access(data_cmd, F_OK) != 0)
		handle_fok_error(p, data_cmd);
	if (access(data_cmd, X_OK) != 0)
		handle_xok_error(p, data_cmd);
	return (data_cmd);
}

char	*get_cmd_name_and_path(t_exec_pack *p,
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

int	handle_single_cmd_parent(t_cmdnode *data_tmp,
		t_cmdnode **data, char ***envp, t_exit_ctx *ctx)
{
	int	return_value;

	return_value = 0;
	if ((data_tmp->cmd == NULL || data_tmp->cmd[0] == '\0'))
		return (execute_only_redirect(data_tmp->infiles, data_tmp->outfiles));
	if ((is_built_in(data_tmp->cmd) == 1))
	{
		return_value = execute_only_built_in((t_builtin_ctx){
				data_tmp->infiles,
				data_tmp->outfiles,
				data,
				envp,
				ctx
			});
		return (return_value);
	}
	return (0);
}
