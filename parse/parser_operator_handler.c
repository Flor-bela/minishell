/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_operator_handler.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 10:13:35 by oscar             #+#    #+#             */
/*   Updated: 2026/03/16 16:16:07 by oscar            ###   ########.fr       */
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
		printf("minishell: syntax error\n");
		change_exit_value("2", &env);
		return (NULL);
	}
	expanded = expand_env_vars(target, env);
	free(target);
	return (expanded);
}

static int	store_operator_target(t_parser *p, char *target, char **env)
{
	if (p->current_cmd)
		return (add_redir_target(p->current_cmd, p->op_code, target, env));
	return (store_pending_redir(p, p->op_code, target, env));
}

int	handle_operator(const char *line, t_parser *p,
	t_intvec *codes, char **env)
{
	char	*target;

	intvec_push(codes, p->op_code);
	p->i += p->op_len;
	if (p->op_code == 5)
	{
		p->current_cmd = NULL;
		return (0);
	}
	target = get_redir_target(line, &p->i, env);
	if (!target)
		return (-1);
	if (store_operator_target(p, target, env))
	{
		free(target);
		return (-1);
	}
	free(target);
	return (0);
}
