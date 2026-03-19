/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_command.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 10:13:03 by oscar             #+#    #+#             */
/*   Updated: 2026/03/19 23:13:15 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

char	*get_clean_token(const char *line, size_t *i, char **env)
{
	char	*tok;
	char	*expanded;
	char	*clean;
	int		grouped;

	grouped = 0;
	tok = read_token_grouped(line, i, &grouped);
	if (!tok)
		return (NULL);
	expanded = expand_env_vars(tok, env);
	free(tok);
	if (!expanded)
		return (NULL);
	clean = remove_quotes_str(expanded);
	free(expanded);
	return (clean);
}

static int	add_command_token(const char *line, t_parser *p, char **env)
{
	char	*clean;

	clean = get_clean_token(line, &p->i, env);
	if (!clean)
		return (1);
	if (clean[0] == '\0')
	{
		free(clean);
		return (0);
	}
	if (!p->current_cmd->cmd)
		p->current_cmd->cmd = ft_strdup(clean);
	else
		str_list_push_back(&p->current_cmd->phrase, clean);
	free(clean);
	return (0);
}

int	handle_command(const char *line, t_parser *p,
				t_intvec *codes, char **env)
{
	if (codes)
	{
	}
	if (!p->current_cmd)
	{
		p->current_cmd = node_new(NULL);
		cmd_list_append(&p->head, &p->tail, p->current_cmd);
	}
	if (add_command_token(line, p, env))
		return (1);
	return (0);
}
