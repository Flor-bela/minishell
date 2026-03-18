/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/17 12:44:37 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/18 11:41:46 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "provisionaryFunctions.h"
/* 
int	builtin_cd(char **args, **envp)
{
	char *path;

	path = NULL;
	if (args == NULL)
	{
		path = getenv("HOME");
		if (path == NULL)
		{
			write(2, "minishell: cd: HOME not set\n", 28); // not sure about his error message
			return (1);
		}
	}
	// else // too many arguments when??? -> check this
		// write(2, "minishell: cd: too many arguments\n", 34);
	if (chdir(path) != 0)
	{
		perror("minishell: cd");
		return (1);
	}
	// update PWD and OLDPWD -> ver esto!!!

	return (0);
} */
