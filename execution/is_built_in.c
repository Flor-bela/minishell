/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   is_built_in.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/03 18:56:12 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/13 14:02:48 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	is_built_in(char *cmd_name)
{
	int	built_in;

	built_in = 0;
	if (ft_strncmp (cmd_name, "echo", 4) == 0 && cmd_name[4] == '\0')
		built_in = 1;
	if (ft_strncmp (cmd_name, "cd", 2) == 0 && cmd_name[2] == '\0')
		built_in = 1;
	if (ft_strncmp (cmd_name, "pwd", 3) == 0 && cmd_name[3] == '\0')
		built_in = 1;
	if (ft_strncmp (cmd_name, "export", 6) == 0 && cmd_name[6] == '\0')
		built_in = 1;
	if (ft_strncmp (cmd_name, "unset", 5) == 0 && cmd_name[5] == '\0')
		built_in = 1;
	if (ft_strncmp (cmd_name, "env", 3) == 0 && cmd_name[3] == '\0')
		built_in = 1;
	if (ft_strncmp (cmd_name, "exit", 4) == 0 && cmd_name[4] == '\0')
		built_in = 1;
	return (built_in);
}
