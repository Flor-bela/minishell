/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_modify.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 08:06:04 by oscar             #+#    #+#             */
/*   Updated: 2026/03/23 09:42:30 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "provisionaryFunctions.h"

int	find_env_index(char **envp, const char *key)
{
	size_t	i;
	size_t	klen;

	if (!envp || !key)
		return (-1);
	klen = key_len(key);
	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], key, klen) == 0
			&& (envp[i][klen] == '=' || envp[i][klen] == '\0'))
			return ((int)i);
		i++;
	}
	return (-1);
}

int	replace_env_value(char **envp, int idx, const char *new_value)
{
	char	*dup;

	dup = ms_strdup(new_value);
	if (!dup)
		return (1);
	free(envp[idx]);
	envp[idx] = dup;
	return (0);
}

static char	**copy_env(char **env, size_t len)
{
	size_t	i;
	char	**new_env;

	new_env = malloc(sizeof(char *) * (len + 2));
	if (!new_env)
		return (NULL);
	i = 0;
	while (i < len)
	{
		new_env[i] = env[i];
		i++;
	}
	return (new_env);
}

int	append_env_value(char ***envp, const char *new_value)
{
	size_t	len;
	char	**new_env;

	len = env_len(*envp);
	new_env = copy_env(*envp, len);
	if (!new_env)
		return (1);
	new_env[len] = ms_strdup(new_value);
	if (!new_env[len])
	{
		free(new_env);
		return (1);
	}
	new_env[len + 1] = NULL;
	free(*envp);
	*envp = new_env;
	return (0);
}

int	remove_env_index(char ***envp, int idx)
{
	size_t	len;
	size_t	i;
	size_t	j;
	char	**new_env;

	if (!envp || !*envp || idx < 0)
		return (0);
	len = env_len(*envp);
	new_env = alloc_env_minus_one(len);
	if (!new_env)
		return (1);
	i = 0;
	j = 0;
	while (i < len)
	{
		if ((int)i != idx)
			new_env[j++] = (*envp)[i];
		else
			free((*envp)[i]);
		i++;
	}
	new_env[j] = NULL;
	free(*envp);
	*envp = new_env;
	return (0);
}
