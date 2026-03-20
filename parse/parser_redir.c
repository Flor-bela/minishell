/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redir.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 10:13:49 by oscar             #+#    #+#             */
/*   Updated: 2026/03/20 17:07:27 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

static int	handle_heredoc(t_cmdnode *cmd,
	const char *target, char **env)
{
	char	*real_delim;
	t_list_red *node;

	(void)env;

	real_delim = remove_quotes_str(target);
	if (!real_delim)
		return (-1);

	// ❌ NO ejecutar heredoc aquí
	// heredoc_with_fork(...)

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

static int	handle_pending_heredoc(t_parser *p,
	const char *target, char **env, t_cmdnode **data, char *line, t_intvec *codes)
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

	// 🔥 CAMBIO CLAVE
	heredoc_content = heredoc_with_fork(real_delim, expand_vars, env, data, line, codes);
	////printf("DEBUG heredoc devuelto = %p\n", heredoc_content);
	free(real_delim);

	if (!heredoc_content)
		return (0);

	red_list_push_back(&p->pending_in, heredoc_content, 1);
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

int	store_pending_redir(t_parser *p, int op_code,
	const char *target, char **env, t_cmdnode **data, char *line, t_intvec *codes)
{
	if (op_code == 1)
		red_list_push_back(&p->pending_in, target, 0);
	else if (op_code == 3)
		return (handle_pending_heredoc(p, target, env, data, line, codes));
	else if (op_code == 2)
		red_list_push_back(&p->pending_out, target, 0);
	else if (op_code == 4)
		red_list_push_back(&p->pending_out, target, 1);
	return (0);
}
