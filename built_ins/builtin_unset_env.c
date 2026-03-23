/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_unset_env.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 08:26:17 by oscar             #+#    #+#             */
/*   Updated: 2026/03/23 08:27:12 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "provisionaryFunctions.h"

static int	process_unset_arg(char *arg, char ***envp)
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

int	builtin_env(char **envp)
{
	size_t	i;

	i = 0;
	while (envp && envp[i])
	{
		printf("%s\n", envp[i]);
		i++;
	}
	return (0);
}
