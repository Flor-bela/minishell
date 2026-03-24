/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_env.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 06:18:06 by oscar             #+#    #+#             */
/*   Updated: 2026/03/24 07:05:33 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	process_export_update(char *arg, char ***envp, int idx)
{
	if (idx >= 0)
	{
		if (replace_env_value(*envp, idx, arg) != 0)
		{
			change_exit_value("1", envp);
			return (1);
		}
	}
	else
	{
		if (append_env_value(envp, arg) != 0)
		{
			change_exit_value("1", envp);
			return (1);
		}
	}
	return (0);
}

int	process_export_arg(char *arg, char ***envp)
{
	int	idx;

	if (!is_valid_identifier(arg))
	{
		write(2, "minishell: export: `", 20);
		write(2, arg, ft_strlen(arg));
		write(2, "': not a valid identifier\n", 26);
		change_exit_value("1", envp);
		return (1);
	}
	if (!ft_strchr(arg, '='))
		return (0);
	idx = find_env_index(*envp, arg);
	return (process_export_update(arg, envp, idx));
}

int	builtin_export(t_list *args, char ***envp)
{
	t_list	*curr;
	char	*arg;

	if (!args)
	{
		print_export_env(*envp);
		return (0);
	}
	curr = args;
	while (curr)
	{
		if (!curr->content)
		{
			curr = curr->next;
			continue ;
		}
		arg = (char *)curr->content;
		if (process_export_arg(arg, envp))
			return (1);
		curr = curr->next;
	}
	return (0);
}

int	process_unset_arg(char *arg, char ***envp)
{
	int	idx;

	if (!is_valid_identifier(arg))
	{
		write(2, "minishell: unset: `", 19);
		write(2, arg, ft_strlen(arg));
		write(2, "': not a valid identifier\n", 26);
		change_exit_value("1", envp);
		return (1);
	}
	idx = find_env_index(*envp, arg);
	if (idx >= 0)
	{
		if (remove_env_index(envp, idx) != 0)
			return (-1);
	}
	return (0);
}

int	builtin_unset(t_list *args, char ***envp)
{
	t_list	*curr;
	char	*arg;
	int		status;

	curr = args;
	while (curr)
	{
		arg = (char *)curr->content;
		status = process_unset_arg(arg, envp);
		if (status == -1)
			return (1);
		curr = curr->next;
	}
	return (0);
}
