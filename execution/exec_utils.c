/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 08:41:53 by oscar             #+#    #+#             */
/*   Updated: 2026/03/23 10:27:51 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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

int	handle_empty_case(char ***envp)
{
	change_exit_value("0", envp);
	return (0);
}
