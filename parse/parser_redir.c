/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redir.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 10:13:49 by oscar             #+#    #+#             */
/*   Updated: 2026/03/23 07:59:21 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

static int	handle_heredoc(t_cmdnode *cmd,
	const char *target, char **env)
{
	char		*real_delim;
	t_list_red	*node;

	(void)env;
	real_delim = remove_quotes_str(target);
	if (!real_delim)
		return (-1);
	node = red_list_push_back(&cmd->infiles, real_delim, 1);
	if (!node)
	{
		free(real_delim);
		return (-1);
	}
	node->is_heredoc = 1;
	free(real_delim);
	return (0);
}

static int	init_heredoc_ctx(t_heredoc_fork *ctx, t_heredoc_args *args)
{
	char	*real_delim;

	ctx->expand = 1;
	if (strchr(args->target, '\'') || strchr(args->target, '"'))
		ctx->expand = 0;
	real_delim = remove_quotes_str(args->target);
	if (!real_delim)
		return (-1);
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
	char			*heredoc_content;

	if (init_heredoc_ctx(&ctx, args) == -1)
		return (-1);
	heredoc_content = heredoc_with_fork(&ctx, args);
	free(ctx.delimiter);
	free((char *)args->target);
	if (!heredoc_content)
		return (0);
	red_list_push_back(&args->parser->pending_in, heredoc_content, 1);
	free(heredoc_content);
	return (0);
}

int	add_redir_target(t_cmdnode *cmd, int op_code,
	const char *target, char **env)
{
	if (op_code == 1)
		red_list_push_back(&cmd->infiles, target, 0);
	else if (op_code == 3)
		return (handle_heredoc(cmd, target, env));
	else if (op_code == 2)
		red_list_push_back(&cmd->outfiles, target, 0);
	else if (op_code == 4)
		red_list_push_back(&cmd->outfiles, target, 1);
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
