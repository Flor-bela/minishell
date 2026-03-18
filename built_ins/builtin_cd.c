/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 16:46:54 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/18 17:39:36 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "provisionaryFunctions.h"
 
static int	update_env(char **envp, char *key, char *path)
{
	size_t	i;
	size_t	len;
	char	*new_entry;
	char	*final_entry;

	if (envp == NULL || key == NULL || path == NULL)
		return (1);

	len = ft_strlen(key);
	i = 0;
	while (envp[i] != NULL)
	{
		if (ft_strncmp(envp[i], key, len) == 0 && envp[i][len] == '=')
		{
			free(envp[i]);
			new_entry = ft_strjoin(key, "=");
			final_entry = ft_strjoin(new_entry, path);
			free(new_entry);
			envp[i] = final_entry;
			return (0);
		}
		i++;
	}
	return (0);
}

int	builtin_cd(t_list *args, char **envp)
{
	char	*path;
	char	*oldpwd;
	char	*newpwd;

	oldpwd = NULL;
	newpwd = NULL;
	if (args != NULL && args->next != NULL)
	{
		write(2, "minishell: cd: too many arguments\n", 35);
		return (1);
	}
	oldpwd = getcwd(NULL, 0);
	if (envp == NULL)
		return (1);
	if (oldpwd == NULL)
		oldpwd = ft_strdup("");
	if (args == NULL)
	{
		path = get_env_value(envp, "HOME");
		if (path == NULL)
		{
			write(2, "minishell: cd: HOME not set\n", 29);
			free(oldpwd);
			return (1);
		}
	}
	else
		path = args->content;
	if (chdir(path) != 0)
	{
		perror("minishell: cd");
		free(oldpwd);
		return (1);
	}
	newpwd = getcwd(NULL, 0);
	if (newpwd == NULL)
		newpwd = ft_strdup("");
	update_env(envp, "OLDPWD", oldpwd);
	update_env(envp, "PWD", newpwd);
	free(oldpwd);
	free(newpwd);
	return (0);
}

