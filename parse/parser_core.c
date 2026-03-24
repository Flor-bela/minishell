/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_core.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 10:13:08 by oscar             #+#    #+#             */
/*   Updated: 2026/03/24 07:05:33 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	parser_init(t_parser *p, t_cmdnode **out_list)
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
	p->last_pipe = 1;
	*out_list = NULL;
}

int	parser_error(t_parser *p, t_intvec *codes)
{
	cmd_list_free(p->head);
	red_list_free(p->pending_in);
	red_list_free(p->pending_out);
	intvec_free(codes);
	return (-1);
}

void	finalize_pending(t_parser *p)
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

int	parse_loop(t_parse_ctx *ctx)
{
	while (ctx->line[ctx->p->i])
	{
		skip_spaces(ctx->line, &ctx->p->i);
		if (!ctx->line[ctx->p->i])
			break ;
		ctx->p->op_match = match_op(ctx->line,
				ctx->p->i, &ctx->p->op_len, &ctx->p->op_code);
		if (ctx->p->op_match == -1)
			return (-1);
		if (ctx->p->op_match == 1)
		{
			if (handle_operator(ctx) == -1)
				return (-1);
		}
		else
		{
			if (handle_command(ctx->line,
					ctx->p, ctx->codes, ctx->env) == -1)
				return (-1);
		}
	}
	finalize_pending(ctx->p);
	return (0);
}
