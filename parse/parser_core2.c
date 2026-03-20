/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_core2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/17 18:01:22 by oscar             #+#    #+#             */
/*   Updated: 2026/03/20 17:49:49 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

static int	error_pipe(t_parser *p, t_intvec *codes)
{
	write(2, "syntax error near unexpected token `|'\n", 40);
	return (parser_error(p, codes));
}

static int	handle_op_line(const char *line, t_parser *p,
	t_intvec *codes, char **env, t_cmdnode **data)
{
	if (p->op_code == 5)
	{
		if (p->last_pipe)
			return (error_pipe(p, codes));
		p->last_pipe = 1;
	}
	else
		p->last_pipe = 0;
	if (handle_operator(line, p, codes, env, data) == -1)
		return (parser_error(p, codes));
	return (0);
}

static int	handle_cmd_line(const char *line, t_parser *p,
	t_intvec *codes, char **env)
{
	p->last_pipe = 0;
	if (handle_command(line, p, codes, env) == -1)
		return (parser_error(p, codes));
	return (0);
}

static int	parse_loop_line(const char *line, t_parser *p,
	t_intvec *codes, char **env, t_cmdnode **data)
{
	p->last_pipe = 1;
	////printf("DEBUG entrando parser\n");
	while (line[p->i])
	{
		skip_spaces(line, &p->i);
		if (!line[p->i])
			break ;

		p->op_match = match_op(line, p->i, &p->op_len, &p->op_code);

		if (p->op_match == -1)
			return (parser_error(p, codes));

		if (p->op_match == 1)
		{
			if (handle_op_line(line, p, codes, env, data))
				return (-1);
		}
		else
		{
			if (handle_cmd_line(line, p, codes, env))
				return (-1);
		}
	}
	if (p->last_pipe)
		return (error_pipe(p, codes));

	return (0);
}

int	parse_line(const char *line, t_intvec *codes,
	t_cmdnode **out_list, char **env)
{
	t_parser	p;

	parser_init(&p, out_list);
	if (parse_loop_line(line, &p, codes, env, out_list) == -1)
		return (-1);
	finalize_pending(&p);
	*out_list = p.head;
	return (0);
}
