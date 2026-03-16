/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_token.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 10:13:59 by oscar             #+#    #+#             */
/*   Updated: 2026/03/16 17:55:07 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

static int	read_token_loop(const char *s, size_t *i,
	char *quote, int *grouped_out)
{
	size_t	op_len;
	int		op_code;

	while (s[*i])
	{
		if (*quote == 0)
		{
			if (match_op(s, *i, &op_len, &op_code) != 0
				|| isspace((unsigned char)s[*i]))
				break;

			if (s[*i] == '\'' || s[*i] == '"')
			{
				*quote = s[*i];
				if (grouped_out)
					*grouped_out = 1;
				(*i)++;
			}
			else
				(*i)++;
		}
		else
		{
			if (s[*i] == *quote)
				*quote = 0;
			(*i)++;
		}
	}
	return (0);
}

char	*read_token_grouped(const char *s, size_t *i, int *grouped_out)
{
	size_t	start;
	size_t	op_len;
	int		op_code;
	int		op_match;
	char	quote;

	if (grouped_out)
		*grouped_out = 0;
	skip_spaces(s, i);
	if (!s[*i])
		return (NULL);
	op_match = match_op(s, *i, &op_len, &op_code);
	if (op_match != 0)
		return (NULL);
	start = *i;
	quote = 0;
	read_token_loop(s, i, &quote, grouped_out);
	return (dup_range(s, start, *i));
}
