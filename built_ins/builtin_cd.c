/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/17 12:44:37 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/17 13:44:55 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "provisionaryFunctions.h"

int	builtin_cd(char *path)
{
	if (path == NULL)
	{
		path = getenv("HOME");
		if (path == NULL)
		{
			write(2, "minishell: cd: ", 16); // minishell
			perror(path);
			return (1);
		}
	}
	if (chdir(path) != 0)
	{
		perror("cd");
		return (1);
	}

	return (0);
}
