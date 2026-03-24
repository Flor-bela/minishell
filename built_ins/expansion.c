/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 05:56:38 by oscar             #+#    #+#             */
/*   Updated: 2026/03/24 14:49:25 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	is_var_start_local(char c)
{
	return (ft_isalpha((unsigned char)c) || c == '_');
}

int	is_var_char_local(char c)
{
	return (ft_isalnum((unsigned char)c) || c == '_');
}

char	*append_char_local(char *base, char c)
{
	size_t	len;
	char	*res;

	if (!base)
	{
		res = malloc(2);
		if (!res)
			return (NULL);
		res[0] = c;
		res[1] = '\0';
		return (res);
	}
	len = ft_strlen(base);
	res = malloc(len + 2);
	if (!res)
	{
		freedom((void **)&base);
		return (NULL);
	}
	ft_memcpy(res, base, len);
	res[len] = c;
	res[len + 1] = '\0';
	freedom((void **)&base);
	return (res);
}

char	*append_str_local(char *base, const char *add)
{
	size_t	len1;
	size_t	len2;
	char	*res;

	if (!add)
		add = "";
	if (!base)
		return (ft_strdup(add));
	len1 = ft_strlen(base);
	len2 = ft_strlen(add);
	res = malloc(len1 + len2 + 1);
	if (!res)
	{
		freedom((void **)&base);
		return (NULL);
	}
	ft_memcpy(res, base, len1);
	ft_memcpy(res + len1, add, len2);
	res[len1 + len2] = '\0';
	freedom((void **)&base);
	return (res);
}

char	*get_var_value(const char *line, size_t *i, char **env)
{
	size_t	start;
	char	*name;
	char	*value;

	start = *i;
	while (line[*i] && is_var_char_local(line[*i]))
		(*i)++;
	name = ft_strndup(line + start, *i - start);
	if (!name)
		return (NULL);
	value = get_env_value(env, name);
	freedom((void **)&name);
	return (value);
}
