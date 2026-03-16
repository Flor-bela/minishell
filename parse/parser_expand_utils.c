/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_expand_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 10:13:15 by oscar             #+#    #+#             */
/*   Updated: 2026/03/16 12:11:35 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

static char	*append_char_new(char c)
{
	char	*res;

	res = malloc(2);
	if (!res)
	{
		perror("malloc");
		exit(1);
	}
	res[0] = c;
	res[1] = 0;
	return (res);
}

char	*append_char_dyn(char *base, char c)
{
	size_t	len;
	char	*res;

	if (!base)
		return (append_char_new(c));
	len = strlen(base);
	res = malloc(len + 2);
	if (!res)
	{
		perror("malloc");
		free(base);
		exit(1);
	}
	memcpy(res, base, len);
	res[len] = c;
	res[len + 1] = 0;
	free(base);
	return (res);
}

static char	*append_str_new(const char *add)
{
	char	*res;

	res = strdup(add);
	if (!res)
	{
		perror("strdup");
		exit(1);
	}
	return (res);
}

char	*append_str_dyn(char *base, const char *add)
{
	size_t	len1;
	size_t	len2;
	char	*res;

	if (!add)
		add = "";
	if (!base)
		return (append_str_new(add));
	len1 = strlen(base);
	len2 = strlen(add);
	res = malloc(len1 + len2 + 1);
	if (!res)
	{
		perror("malloc");
		free(base);
		exit(1);
	}
	memcpy(res, base, len1);
	memcpy(res + len1, add, len2);
	res[len1 + len2] = 0;
	free(base);
	return (res);
}
