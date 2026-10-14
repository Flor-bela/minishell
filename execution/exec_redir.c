/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redir.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 23:01:53 by fda-roch          #+#    #+#             */
/*   Updated: 2026/10/14 23:36:55 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	handle_redirects(t_list_red *data_infiles,
	t_list_red *data_outfiles, int *saved_stdout)
{
	int	return_value;

	return_value = 0;
	if (data_infiles != NULL)
		return_value = handle_infile_redirections(&data_infiles);
	if (data_outfiles != NULL)
	{
		*saved_stdout = dup(STDOUT_FILENO);
		return_value = handle_outfile_redirections(&data_outfiles);
	}
	return (return_value);
}

int	execute_only_redirect(t_list_red *data_infiles,
	t_list_red *data_outfiles)
{
	int	saved_stdin;
	int	saved_stdout;
	int	return_value;

	return_value = 0;
	saved_stdin = -1;
	saved_stdout = -1;
	saved_stdin = dup(STDIN_FILENO);
	return_value = handle_redirects(data_infiles,
			data_outfiles, &saved_stdout);
	restore_fds(saved_stdin, saved_stdout);
	return (return_value);
}

int	handle_outfile_redirections(t_list_red **tmp_outfiles)
{
	int		fd_file;
	char	*error;

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
			error = ft_strjoin_modif("minishell: ", (*tmp_outfiles)->content);
			perror(error);
			return (1);
		}
		dup2(fd_file, STDOUT_FILENO);
		close(fd_file);
		*tmp_outfiles = (*tmp_outfiles)->next;
	}
	return (0);
}

void	restore_fds(int saved_stdin, int saved_stdout)
{
	if (saved_stdin != -1)
	{
		dup2(saved_stdin, STDIN_FILENO);
		close(saved_stdin);
	}
	if (saved_stdout != -1)
	{
		dup2(saved_stdout, STDOUT_FILENO);
		close(saved_stdout);
	}
}
