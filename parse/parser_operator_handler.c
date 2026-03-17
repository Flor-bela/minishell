/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_operator_handler.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 10:13:35 by oscar             #+#    #+#             */
/*   Updated: 2026/03/17 17:16:36 by oscar            ###   ########.fr       */
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

	// 🔥 CASO PIPE
	if (p->op_code == 5)
	{
		// mover redirecciones pendientes al nodo actual
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

		// 🔥 CREAR NUEVO NODO SIEMPRE
		p->current_cmd = node_new(NULL);
		cmd_list_append(&p->head, &p->tail, p->current_cmd);

		return (0);
	}

	// 🔥 REDIRECCIONES
	target = get_redir_target(line, &p->i, env);
	if (!target)
		return (-1);

	if (p->current_cmd)
	{
		if (add_redir_target(p->current_cmd, p->op_code, target, env))
		{
			free(target);
			return (-1);
		}
	}
	else
	{
		if (store_pending_redir(p, p->op_code, target, env))
		{
			free(target);
			return (-1);
		}
	}

	free(target);
	return (0);
}