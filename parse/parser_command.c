/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_command.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 10:13:03 by oscar             #+#    #+#             */
/*   Updated: 2026/03/17 17:32:50 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

static int	add_command_token(const char *line, t_parser *p, char **env)
{
	int		grouped;
	char	*tok;
	char	*expanded;
	char	*clean;

	grouped = 0;
	tok = read_token_grouped(line, &p->i, &grouped);
	if (!tok)
		return (1);

	expanded = expand_env_vars(tok, env);
	free(tok);
	if (!expanded)
		return (1);

	clean = remove_quotes_str(expanded);
	free(expanded);
	if (!clean)
		return (1);

	// 🔥 CASO CLAVE: token vacío → ignorar completamente
	if (clean[0] == '\0')
	{
		free(clean);
		return (0);
	}

	// 🔥 SI NO HAY CMD → ESTE ES EL CMD REAL
	if (!p->current_cmd->cmd)
	{
		p->current_cmd->cmd = ft_strdup(clean);
	}
	else
	{
		str_list_push_back(&p->current_cmd->phrase, clean);
	}

	free(clean);
	return (0);
}

static int	handle_tail_operator(const char *line, t_parser *p,
	t_intvec *codes, char **env)
{
	if (p->op_code == 5)
	{
		intvec_push(codes, p->op_code);
		p->i = p->j + p->op_len;
		p->current_cmd = NULL;
		return (0);
	}
	p->i = p->j;
	return (handle_operator(line, p, codes, env));
}

static int	fill_command_tail(const char *line, t_parser *p,
	t_intvec *codes, char **env)
{
	while (line[p->i])
	{
		p->j = p->i;
		skip_spaces(line, &p->j);
		if (!line[p->j])
		{
			p->i = p->j;
			return (0);
		}
		p->op_match = match_op(line, p->j, &p->op_len, &p->op_code);
		if (p->op_match == -1)
			return (-1);
		if (p->op_match == 1)
			return (handle_tail_operator(line, p, codes, env));
		p->i = p->j;
		if (add_command_token(line, p, env))
			return (0);
	}
	return (0);
}

int	handle_command(const char *line, t_parser *p,
				t_intvec *codes, char **env)
{
	if (!p->current_cmd)
	{
		p->current_cmd = node_new(NULL);
		cmd_list_append(&p->head, &p->tail, p->current_cmd);
	}

	if (add_command_token(line, p, env))
		return (1);

	return (0);
}