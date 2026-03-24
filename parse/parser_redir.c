/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redir.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkalgano <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 10:13:49 by oscar             #+#    #+#             */
/*   Updated: 2026/03/24 17:14:13 by dkalgano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	init_heredoc_ctx2(t_heredoc_fork *ctx, t_heredoc_args *args)
{
	char	*real_delim;

	ctx->expand = 1;
	if (ft_strchr(args->target, '\'') || ft_strchr(args->target, '"'))
		ctx->expand = 0;
	real_delim = remove_quotes_str(args->target);
	if (!real_delim)
	{
		return (-1);
	}
	ctx->delimiter = real_delim;
	ctx->env = args->env;
	ctx->data = args->data;
	ctx->line = args->line;
	ctx->codes = args->codes;
	return (0);
}

static int	handle_pending_heredoc(t_heredoc_args *args)
{
	t_heredoc_fork	ctx;
	int				fd;

	if (init_heredoc_ctx2(&ctx, args) == -1)
		return (-1);
	fd = heredoc_with_fork(&ctx, args);
	freedom((void **)&ctx.delimiter);
	freedom((void **)&args->target);
	if (fd == -1)
		return (-1);
	red_list_push_back(&args->parser->pending_in, NULL, fd);
	return (0);
}

static int	handle_redir_heredoc(t_redir_params *p)
{
	t_heredoc_args	args;
	t_heredoc_fork	ctx2;
	int				fd;

	args.parser = p->ctx->p;
	args.target = p->target;
	args.env = p->ctx->env;
	args.data = p->ctx->data;
	args.line = (char *)p->ctx->line;
	args.codes = p->ctx->codes;
	if (init_heredoc_ctx2(&ctx2, &args) == -1)
		return (-1);
	fd = heredoc_with_fork(&ctx2, &args);
	freedom((void **)&ctx2.delimiter);
	freedom((void **)&args.target);
	red_list_push_back(&p->cmd->infiles, NULL, fd);
	return (0);
}

int	add_redir_target(t_redir_params *p)
{
	if (p->op_code == 1)
		red_list_push_back(&p->cmd->infiles, p->target, 0);
	else if (p->op_code == 3)
		return (handle_redir_heredoc(p));
	else if (p->op_code == 2)
		red_list_push_back(&p->cmd->outfiles, p->target, 0);
	else if (p->op_code == 4)
		red_list_push_back(&p->cmd->outfiles, p->target, 1);
	return (0);
}

int	store_pending_redir(t_parse_ctx *ctx, const char *target)
{
	t_heredoc_args	args;

	if (ctx->p->op_code == 1)
		red_list_push_back(&ctx->p->pending_in, target, 0);
	else if (ctx->p->op_code == 3)
	{
		args.parser = ctx->p;
		args.target = target;
		args.env = ctx->env;
		args.data = ctx->data;
		args.line = (char *)ctx->line;
		args.codes = ctx->codes;
		return (handle_pending_heredoc(&args));
	}
	else if (ctx->p->op_code == 2)
		red_list_push_back(&ctx->p->pending_out, target, 0);
	else if (ctx->p->op_code == 4)
		red_list_push_back(&ctx->p->pending_out, target, 1);
	return (0);
}
