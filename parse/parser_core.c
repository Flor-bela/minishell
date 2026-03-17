/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_core.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 10:13:08 by oscar             #+#    #+#             */
/*   Updated: 2026/03/17 17:12:34 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

static void	parser_init(t_parser *p, t_cmdnode **out_list)
{
	p->i = 0;
	p->j = 0;
	p->head = NULL;
	p->tail = NULL;
	p->current_cmd = NULL;
	p->pending_in = NULL;
	p->pending_out = NULL;
	p->op_len = 0;
	p->op_code = -1;
	p->op_match = 0;
	*out_list = NULL;
}

static int	parser_error(t_parser *p, t_intvec *codes)
{
	cmd_list_free(p->head);
	red_list_free(p->pending_in);
	red_list_free(p->pending_out);
	intvec_free(codes);
	return (-1);
}

static void	finalize_pending(t_parser *p)
{
	if (p->pending_in || p->pending_out)
	{
		p->current_cmd = node_new(NULL);
		move_red_list_to_back(&p->current_cmd->infiles, &p->pending_in);
		move_red_list_to_back(&p->current_cmd->outfiles, &p->pending_out);
		cmd_list_append(&p->head, &p->tail, p->current_cmd);
	}
	p->current_cmd = p->head;
}

static int	parse_loop(const char *line, t_parser *p,
	t_intvec *codes, char **env)
{
	while (line[p->i])
	{
		skip_spaces(line, &p->i);
		if (!line[p->i])
			break ;
		p->op_match = match_op(line, p->i, &p->op_len, &p->op_code);
		if (p->op_match == -1)
			return (-1);
		if (p->op_match == 1)
		{
			if (handle_operator(line, p, codes, env) == -1)
				return (-1);
		}
		else
		{
			if (handle_command(line, p, codes, env) == -1)
				return (-1);
		}
	}
	finalize_pending(p);
	return (0);
}

int	parse_line(const char *line, t_intvec *codes,
			t_cmdnode **out_list, char **env)
{
	t_parser	p;
	int			last_was_pipe;

	parser_init(&p, out_list);
	last_was_pipe = 1;

	while (line[p.i])
	{
		skip_spaces(line, &p.i);
		if (!line[p.i])
			break ;

		// 🔥 DETECTAR OPERADOR
		p.op_match = match_op(line, p.i, &p.op_len, &p.op_code);
		if (p.op_match == -1)
			return (parser_error(&p, codes));

		if (p.op_match == 1)
		{
			// 🔥 CONTROL DE PIPE
			if (p.op_code == 5)
			{
				if (last_was_pipe)
				{
					write(2, "syntax error near unexpected token `|'\n", 40);
					return (parser_error(&p, codes));
				}
				last_was_pipe = 1;
			}
			else
				last_was_pipe = 0;

			if (handle_operator(line, &p, codes, env) == -1)
				return (parser_error(&p, codes));
		}
		else
		{
			last_was_pipe = 0;

			if (handle_command(line, &p, codes, env) == -1)
				return (parser_error(&p, codes));
		}
	}

	// 🔥 ERROR SI TERMINA EN PIPE
	if (last_was_pipe)
	{
		write(2, "syntax error near unexpected token `|'\n", 40);
		return (parser_error(&p, codes));
	}

	finalize_pending(&p);
	*out_list = p.head;
	return (0);
}