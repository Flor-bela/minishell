/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_operator_handler.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 10:13:35 by oscar             #+#    #+#             */
/*   Updated: 2026/03/24 16:23:53 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*get_redir_target(const char *line, size_t *i,
				char **env, int op_code)
{
	char	*target;
	char	*expanded;
	int		grouped;
	char	*clean;

	expanded = NULL;
	grouped = 0;
	target = read_token_grouped(line, i, &grouped);
	if (!target)
		return (change_exit_value("2", &env), NULL);
	if (op_code == 3 && is_quoted(target))
	{
		clean = ft_strdup(target);
		freedom((void **)&target);
		return (clean);
	}
	expanded = expand_env_vars(target, env);
	freedom((void **)&target);
	if (!expanded)
		return (NULL);
	clean = ft_strdup(expanded);
	freedom((void **)&expanded);
	return (clean);
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

int	handle_pipe1(t_parser *p)
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

static int	handle_redir_error(char *target)
{
	freedom((void **)&target);
	return (-1);
}

int	handle_redirection(t_parse_ctx *ctx)
{
	char			*target;
	t_redir_params	p;

	target = get_redir_target(ctx->line, &ctx->p->i, ctx->env, ctx->p->op_code);
	if (!target)
		return (-1);
	if (ctx->p->current_cmd)
	{
		p.ctx = ctx;
		p.cmd = ctx->p->current_cmd;
		p.op_code = ctx->p->op_code;
		p.target = target;
		if (add_redir_target(&p))
			return (handle_redir_error(target));
	}
	else if (store_pending_redir(ctx, target))
		return (handle_redir_error(target));
	return (0);
}
