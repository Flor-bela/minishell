/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_operator.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 10:13:42 by oscar             #+#    #+#             */
/*   Updated: 2026/03/19 16:19:53 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

static int	handle_less(const char *s, size_t i, size_t *len, int *code)
{
	if (s[i + 1] == '<')
	{
		if (s[i + 2] == '<')
		{
			write(2,
				"Error: no se permiten mas de 2 redirecciones '<' seguidas.\n",
				ft_strlen("Error: no se permiten mas de 2 redirecciones '<' seguidas.\n"));
			return (-1);
		}
		*len = 2;
		*code = 3;
		return (1);
	}
	*len = 1;
	*code = 1;
	return (1);
}

static int	handle_greater(const char *s, size_t i, size_t *len, int *code)
{
	if (s[i + 1] == '>')
	{
		if (s[i + 2] == '>')
		{
			write(2,
				"Error: no se permiten mas de 2 redirecciones '>' seguidas.\n",
				ft_strlen("Error: no se permiten mas de 2 redirecciones '>' seguidas.\n"));
			return (-1);
		}
		*len = 2;
		*code = 4;
		return (1);
	}
	*len = 1;
	*code = 2;
	return (1);
}

static int	handle_pipe(const char *s, size_t i, size_t *len, int *code)
{
	if (s[i + 1] == '|')
	{
		write(2, "Error: no se permiten 2 o mas pipes seguidos.\n", 47);
		return (-1);
	}
	*len = 1;
	*code = 5;
	return (1);
}

int	match_op(const char *s, size_t i, size_t *len, int *code)
{
	if (s[i] == '<')
		return (handle_less(s, i, len, code));
	if (s[i] == '>')
		return (handle_greater(s, i, len, code));
	if (s[i] == '|')
		return (handle_pipe(s, i, len, code));
	return (0);
}
