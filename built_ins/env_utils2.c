/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils2.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 05:48:42 by oscar             #+#    #+#             */
/*   Updated: 2026/03/24 13:02:24 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	replace_env_value(char **envp, int idx, const char *new_value)
{
	char	*dup;

	dup = ft_strdup(new_value);
	if (!dup)
		return (1);
	freedom((void **)&envp[idx]);
	envp[idx] = dup;
	return (0);
}

char	**copy_env(char **env, size_t len)
{
	size_t	i;
	char	**new_env;

	i = 0;
	new_env = malloc(sizeof(char *) * (len + 2));
	if (!new_env)
		return (NULL);
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
	new_env[len] = ft_strdup(new_value);
	if (!new_env[len])
		return (free(new_env), 1);
	new_env[len + 1] = NULL;
	freedom((void **)&*envp);
	*envp = new_env;
	return (0);
}

char	**alloc_env_minus_one(size_t len)
{
	return (malloc(sizeof(char *) * len));
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
			freedom((void **)&(*envp)[i]);
		i++;
	}
	new_env[j] = NULL;
	freedom((void **)&*envp);
	*envp = new_env;
	return (0);
}
