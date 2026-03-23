/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 08:09:32 by oscar             #+#    #+#             */
/*   Updated: 2026/03/23 10:20:49 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "provisionaryFunctions.h"

char	*join_and_free(char *s1, const char *s2)
{
	size_t	len1;
	size_t	len2;
	char	*res;

	if (!s1)
	{
		res = ft_strdup(s2);
		if (!res)
			return (NULL);
		return (res);
	}
	len1 = ft_strlen(s1);
	len2 = ft_strlen(s2);
	res = malloc(len1 + len2 + 1);
	if (!res)
	{
		free(s1);
		return (NULL);
	}
	ft_memcpy(res, s1, len1);
	ft_memcpy(res + len1, s2, len2);
	res[len1 + len2] = '\0';
	free(s1);
	return (res);
}

static void	handle_quote_char(const char *str, size_t *j,
								char *quote, char *res)
{
	if (*quote == 0)
		*quote = str[0];
	else if (*quote == str[0])
		*quote = 0;
	else
		res[(*j)++] = str[0];
}

char	*remove_quotes_str(const char *str)
{
	size_t	i;
	size_t	j;
	char	quote;
	char	*res;

	if (!str)
		return (NULL);
	res = malloc(ft_strlen(str) + 1);
	if (!res)
		return (NULL);
	i = 0;
	j = 0;
	quote = 0;
	while (str[i])
	{
		if (str[i] == '"' || str[i] == '\'')
			handle_quote_char(&str[i], &j, &quote, res);
		else
			res[j++] = str[i];
		i++;
	}
	res[j] = '\0';
	return (res);
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
		free(base);
		return (NULL);
	}
	ft_memcpy(res, base, len);
	res[len] = c;
	res[len + 1] = '\0';
	free(base);
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
		free(base);
		return (NULL);
	}
	ft_memcpy(res, base, len1);
	ft_memcpy(res + len1, add, len2);
	res[len1 + len2] = '\0';
	free(base);
	return (res);
}
