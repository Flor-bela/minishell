/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_token_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 10:13:54 by oscar             #+#    #+#             */
/*   Updated: 2026/03/17 12:30:39 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

void	skip_spaces(const char *s, size_t *i)
{
	while (s[*i] && isspace((unsigned char)s[*i]))
		(*i)++;
}

char	*dup_range(const char *s, size_t a, size_t b)
{
	size_t	n;
	char	*out;

	if (b < a)
		b = a;
	n = b - a;
	out = malloc(n + 1);
	if (!out)
	{
		perror("malloc");
		exit(1);
	}
	ft_memcpy(out, s + a, n);
	out[n] = 0;
	return (out);
}

void	update_token_quote(const char *s, size_t *i,
	char *quote, int *grouped_out)
{
	if (s[*i] == *quote)
		*quote = 0;
	else
	{
		*quote = s[*i];
		if (grouped_out)
			*grouped_out = 1;
	}
	(*i)++;
}
