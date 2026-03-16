/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redir.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 10:13:49 by oscar             #+#    #+#             */
/*   Updated: 2026/03/16 10:42:18 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

static int	handle_heredoc(t_cmdnode *cmd,
	const char *target, char **env)
{
	char	*heredoc_content;
	char	*real_delim;
	int		expand_vars;

	expand_vars = 1;
	if (strchr(target, '\'') || strchr(target, '"'))
		expand_vars = 0;
	real_delim = remove_quotes_str(target);
	if (!real_delim)
		return (-1);
	heredoc_content = read_heredoc_input(real_delim, expand_vars, env);
	free(real_delim);
	if (!heredoc_content)
		return (-1);
	red_list_push_back(&cmd->infiles, heredoc_content, 1);
	free(heredoc_content);
	return (0);
}

static int	handle_pending_heredoc(t_parser *p,
	const char *target, char **env)
{
	char	*heredoc_content;
	char	*real_delim;
	int		expand_vars;

	expand_vars = 1;
	if (strchr(target, '\'') || strchr(target, '"'))
		expand_vars = 0;
	real_delim = remove_quotes_str(target);
	if (!real_delim)
		return (-1);
	heredoc_content = read_heredoc_input(real_delim, expand_vars, env);
	free(real_delim);
	if (!heredoc_content)
		return (-1);
	red_list_push_back(&p->pending_in, heredoc_content, 1);
	free(heredoc_content);
	return (0);
}

int	add_redir_target(t_cmdnode *cmd, int op_code,
	const char *target, char **env)
{
	/*if (!cmd)
	{
		fprintf(stderr, "Error: redirección sin comando asociado.\n");
		return (-1);
	}*/
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

int	store_pending_redir(t_parser *p, int op_code,
	const char *target, char **env)
{
	if (op_code == 1)
		red_list_push_back(&p->pending_in, target, 0);
	else if (op_code == 3)
		return (handle_pending_heredoc(p, target, env));
	else if (op_code == 2)
		red_list_push_back(&p->pending_out, target, 0);
	else if (op_code == 4)
		red_list_push_back(&p->pending_out, target, 1);
	return (0);
}
