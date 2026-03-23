/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_core2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/17 18:01:22 by oscar             #+#    #+#             */
/*   Updated: 2026/03/23 10:23:38 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

int	error_pipe(t_parser *p, t_intvec *codes)
{
	write(2, "syntax error near unexpected token `|'\n", 40);
	return (parser_error(p, codes));
}

void	error_pipe2(t_exec_pack *p)
{
	perror("minishell: pipe");
	change_exit_value("1", &p->envp);
	free_envp_copy(p->envp);
	cmd_list_destroy(*p->data);
	free(p->line);
	intvec_destroy(p->codes);
	exit(1);
}

static int	handle_op_line(t_parse_ctx *ctx)
{
	if (ctx->p->op_code == 5)
	{
		if (ctx->p->last_pipe)
			return (error_pipe(ctx->p, ctx->codes));
		ctx->p->last_pipe = 1;
	}
	else
		ctx->p->last_pipe = 0;
	if (handle_operator(ctx) == -1)
		return (parser_error(ctx->p, ctx->codes));
	return (0);
}

static int	handle_cmd_line(t_parse_ctx *ctx)
{
	ctx->p->last_pipe = 0;
	if (handle_command(ctx->line,
			ctx->p, ctx->codes, ctx->env) == -1)
		return (parser_error(ctx->p, ctx->codes));
	return (0);
}

static int	parse_loop_line(t_parse_ctx *ctx)
{
	ctx->p->last_pipe = 1;
	while (ctx->line[ctx->p->i])
	{
		skip_spaces(ctx->line, &ctx->p->i);
		if (!ctx->line[ctx->p->i])
			break ;
		ctx->p->op_match = match_op(ctx->line,
				ctx->p->i, &ctx->p->op_len, &ctx->p->op_code);
		if (ctx->p->op_match == -1)
			return (parser_error(ctx->p, ctx->codes));
		if (ctx->p->op_match == 1)
		{
			if (handle_op_line(ctx))
				return (-1);
		}
		else
		{
			if (handle_cmd_line(ctx))
				return (-1);
		}
	}
	if (ctx->p->last_pipe)
		return (error_pipe(ctx->p, ctx->codes));
	return (0);
}

int	parse_line(const char *line, t_intvec *codes,
	t_cmdnode **out_list, char **env)
{
	t_parser	p;
	t_parse_ctx	ctx;

	parser_init(&p, out_list);
	ctx.line = line;
	ctx.p = &p;
	ctx.codes = codes;
	ctx.env = env;
	ctx.data = out_list;
	if (parse_loop_line(&ctx) == -1)
		return (-1);
	finalize_pending(&p);
	*out_list = p.head;
	return (0);
}
