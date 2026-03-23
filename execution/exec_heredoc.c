/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_heredoc.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 08:58:34 by oscar             #+#    #+#             */
/*   Updated: 2026/03/23 09:27:02 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	exec_heredoc(t_list_red *redir, t_exec_pack *p)
{
	t_heredoc_fork	ctx;
	char			*content;
	int				pipefd[2];

	ctx.delimiter = redir->content;
	ctx.expand = 1;
	ctx.env = p->envp;
	ctx.data = p->data;
	ctx.line = p->line;
	ctx.codes = p->codes;
	content = heredoc_with_fork(&ctx, NULL);
	if (!content)
		handle_heredoc_error(p);
	pipe(pipefd);
	write(pipefd[1], content, ft_strlen(content));
	close(pipefd[1]);
	dup2(pipefd[0], STDIN_FILENO);
	close(pipefd[0]);
	free(content);
}