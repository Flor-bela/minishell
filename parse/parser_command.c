/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_command.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 10:13:03 by oscar             #+#    #+#             */
/*   Updated: 2026/03/16 16:53:04 by oscar            ###   ########.fr       */
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
	clean = remove_quotes_str(expanded);
	free(expanded);
	str_list_push_back(&p->current_cmd->phrase, clean);
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
	char	*cmd;
	char	*expanded;
	int		grouped;

	grouped = 0;
	cmd = read_token_grouped(line, &p->i, &grouped);
	if (!cmd)
		return (0);
	expanded = expand_env_vars(cmd, env);
	free(cmd);
	cmd = remove_quotes_str(expanded);
	free(expanded);
	intvec_push(codes, 0);
	p->current_cmd = node_new(cmd);
	move_red_list_to_back(&p->current_cmd->infiles, &p->pending_in);
	move_red_list_to_back(&p->current_cmd->outfiles, &p->pending_out);
	cmd_list_append(&p->head, &p->tail, p->current_cmd);
	free(cmd);
	return (fill_command_tail(line, p, codes, env));
}
