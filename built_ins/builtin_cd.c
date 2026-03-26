/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <fda-roch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/17 12:44:37 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/24 21:14:15 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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
			freedom((void **)&envp[i]);
			new_entry = ft_strjoin(key, "=");
			final_entry = ft_strjoin(new_entry, path);
			freedom((void **)&new_entry);
			envp[i] = final_entry;
			return (0);
		}
		i++;
	}
	return (0);
}

static char	*cd_get_path(t_list *args, char **envp, char *oldpwd)
{
	char	*path;

	if (!args)
	{
		path = get_env_value(envp, "HOME");
		if (!path)
		{
			write(2, "minishell: cd: HOME not set\n", 29);
			freedom((void **)&oldpwd);
			return (NULL);
		}
	}
	else
		path = args->content;
	return (path);
}

static int	cd_change_dir(char *path, char *oldpwd)
{
	if (chdir(path) != 0)
	{
		write(2, "minishell: cd: ", 16);
		write(2, path, ft_strlen(path));
		write(2, ": ", 2);
		write(2, strerror(errno), ft_strlen(strerror(errno)));
		write(2, "\n", 1);
		freedom((void **)&oldpwd);
		return (1);
	}
	return (0);
}

static void	cd_update_env(char **envp, char *oldpwd)
{
	char	*newpwd;

	newpwd = getcwd(NULL, 0);
	if (!newpwd)
		newpwd = ft_strdup("");
	update_env(envp, "OLDPWD", oldpwd);
	update_env(envp, "PWD", newpwd);
	freedom((void **)&oldpwd);
	freedom((void **)&newpwd);
}

int	builtin_cd(t_list *args, char **envp)
{
	char	*path;
	char	*oldpwd;

	oldpwd = NULL;
	if (args && args->next)
	{
		write(2, "minishell: cd: too many arguments\n", 35);
		return (1);
	}
	oldpwd = getcwd(NULL, 0);
	if (!envp)
		return (1);
	if (!oldpwd)
		oldpwd = ft_strdup("");
	path = cd_get_path(args, envp, oldpwd);
	if (!path)
		return (1);
	if (cd_change_dir(path, oldpwd))
		return (1);
	cd_update_env(envp, oldpwd);
	return (0);
}
