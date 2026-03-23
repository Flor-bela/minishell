/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redirections.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 08:44:26 by oscar             #+#    #+#             */
/*   Updated: 2026/03/23 09:29:57 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	exec_infile(t_list_red *redir, t_exec_pack *p)
{
	int	fd_file;

	fd_file = open(redir->content, O_RDONLY);
	if (fd_file == -1)
	{
		close_fds(p->arr_fds, p->len);
		error_fd_file(redir->content, p);
	}
	dup2(fd_file, STDIN_FILENO);
	close(fd_file);
}

void	handle_infile_redirections(t_list_red **tmp_infiles,
	t_exec_pack *p)
{
	while (*tmp_infiles != NULL)
	{
		if ((*tmp_infiles)->doble == 1)
			exec_heredoc(*tmp_infiles, p);
		else
			exec_infile(*tmp_infiles, p);
		*tmp_infiles = (*tmp_infiles)->next;
	}
}

void	handle_outfile_redirections(t_list_red **tmp_outfiles,
	t_exec_pack *p)
{
	int	fd_file;

	while (*tmp_outfiles != NULL)
	{
		if ((*tmp_outfiles)->doble == 0)
			fd_file = open((*tmp_outfiles)->content,
					O_WRONLY | O_CREAT | O_TRUNC, 0666);
		else
			fd_file = open((*tmp_outfiles)->content,
					O_WRONLY | O_CREAT | O_APPEND, 0666);
		if (fd_file == -1)
		{
			close_fds(p->arr_fds, p->len);
			error_fd_file((*tmp_outfiles)->content, p);
		}
		dup2(fd_file, STDOUT_FILENO);
		close(fd_file);
		*tmp_outfiles = (*tmp_outfiles)->next;
	}
}
