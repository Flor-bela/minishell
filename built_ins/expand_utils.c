/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 08:12:56 by oscar             #+#    #+#             */
/*   Updated: 2026/03/23 10:21:32 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "provisionaryFunctions.h"

static int	is_var_start_local(char c)
{
	return (isalpha((unsigned char)c) || c == '_');
}

static int	is_var_char_local(char c)
{
	return (isalnum((unsigned char)c) || c == '_');
}

static char	*get_var_value(const char *line, size_t *i, char **env)
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
	free(name);
	return (value);
}

static char	*expand_dollar(const char *line, size_t *i,
							char *res, char **shell_env)
{
	char	*value;

	if (!line[*i])
		return (append_char_local(res, '$'));
	if (line[*i] == '?')
	{
		value = get_env_value(shell_env, "cp_envp");
		if (value)
			res = append_str_local(res, value);
		(*i)++;
		return (res);
	}
	if (!is_var_start_local(line[*i]))
		return (append_char_local(res, '$'));
	value = get_var_value(line, i, shell_env);
	if (value)
		res = append_str_local(res, value);
	return (res);
}

char	*expand_heredoc_line(const char *line, char **shell_env)
{
	size_t	i;
	char	*res;

	i = 0;
	res = ft_strdup("");
	if (!res)
		return (NULL);
	while (line[i])
	{
		if (line[i] == '$')
		{
			i++;
			res = expand_dollar(line, &i, res, shell_env);
		}
		else
		{
			res = append_char_local(res, line[i]);
			i++;
		}
		if (!res)
			return (NULL);
	}
	return (res);
}
