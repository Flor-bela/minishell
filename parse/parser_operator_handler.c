/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_operator_handler.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 10:13:35 by oscar             #+#    #+#             */
/*   Updated: 2026/03/21 05:25:07 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

static char	*get_redir_target(const char *line, size_t *i, char **env)
{
	char	*target;
	char	*expanded;
	int		grouped;

	grouped = 0;
	target = read_token_grouped(line, i, &grouped);
	if (!target)
	{
		//printf("minishell: syntax error\n");
		change_exit_value("2", &env);
		return (NULL);
	}
	expanded = expand_env_vars(target, env);
	free(target);
	return (expanded);
}

void	redir_add_back(t_list_red **lst, char *content, int doble)
{
	t_list_red	*new;
	t_list_red	*tmp;

	if (!content)
		return ;

	new = malloc(sizeof(t_list_red));
	if (!new)
		return ;

	new->content = content;
	new->doble = doble;
	new->next = NULL;

	if (*lst == NULL)
	{
		*lst = new;
		return ;
	}

	tmp = *lst;
	while (tmp->next != NULL)
		tmp = tmp->next;

	tmp->next = new;
}

static int	handle_pipe(t_parser *p)
{
	if (p->pending_in || p->pending_out)
	{
		if (!p->current_cmd)
		{
			p->current_cmd = node_new(NULL);
			cmd_list_append(&p->head, &p->tail, p->current_cmd);
		}
		move_red_list_to_back(&p->current_cmd->infiles, &p->pending_in);
		move_red_list_to_back(&p->current_cmd->outfiles, &p->pending_out);
	}
	p->current_cmd = node_new(NULL);
	cmd_list_append(&p->head, &p->tail, p->current_cmd);
	return (0);
}

static int	handle_redirection(t_parse_ctx *ctx)
{
	char	*target;

	target = get_redir_target(ctx->line, &ctx->p->i, ctx->env);
	if (!target)
		return (-1);
	if (ctx->p->current_cmd)
	{
		if (add_redir_target(ctx->p->current_cmd,
				ctx->p->op_code, target, ctx->env))
		{
			free(target);
			return (-1);
		}
	}
	else
	{
		if (store_pending_redir(ctx, target))
		{
			free(target);
			return (-1);
		}
	}
	free(target);
	return (0);
}


int	handle_operator(t_parse_ctx *ctx)
{
	intvec_push(ctx->codes, ctx->p->op_code);
	ctx->p->i += ctx->p->op_len;
	if (ctx->p->op_code == 5)
		return (handle_pipe(ctx->p));
	return (handle_redirection(ctx));
}