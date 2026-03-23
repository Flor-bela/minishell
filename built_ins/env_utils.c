/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 08:03:07 by oscar             #+#    #+#             */
/*   Updated: 2026/03/23 08:05:13 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "provisionaryFunctions.h"

size_t	env_len(char **envp)
{
	size_t	i;

	i = 0;
	while (envp && envp[i])
		i++;
	return (i);
}

char	**dup_envp(char **envp)
{
	size_t	len;
	char	**copy;
	size_t	i;

	len = env_len(envp);
	copy = (char **)malloc(sizeof(char *) * (len + 1));
	if (!copy)
		return (NULL);
	i = 0;
	while (i < len)
	{
		copy[i] = ms_strdup(envp[i]);
		if (!copy[i])
		{
			while (i > 0)
				free(copy[--i]);
			free(copy);
			return (NULL);
		}
		i++;
	}
	copy[len] = NULL;
	return (copy);
}

char	*get_env_value(char **envp, const char *key)
{
	size_t	i;
	size_t	len;

	if (!envp || !key)
		return (NULL);
	len = ft_strlen(key);
	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], key, len) == 0 && envp[i][len] == '=')
			return (envp[i] + len + 1);
		i++;
	}
	return (NULL);
}

void	free_envp_copy(char **envp)
{
	size_t	i;

	if (!envp)
		return ;
	i = 0;
	while (envp[i])
	{
		free(envp[i]);
		i++;
	}
	free(envp);
}

void	change_exit_value(char *value, char ***envp)
{
	int		idx;
	char	*entry;

	if (!value)
		value = "0";
	entry = ft_strjoin("return_value=", value);
	if (!entry)
		return ;
	idx = find_env_index(*envp, "return_value");
	if (idx >= 0)
		replace_env_value(*envp, idx, entry);
	else
		append_env_value(envp, entry);
	free(entry);
}
