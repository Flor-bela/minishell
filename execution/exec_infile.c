/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_infile.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 23:19:29 by fda-roch          #+#    #+#             */
/*   Updated: 2026/10/14 23:36:36 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	handle_infile_doble(t_list_red *infile)
{
	dup2(infile->doble, STDIN_FILENO);
	close(infile->doble);
}

int	handle_infile_file(t_list_red *infile)
{
	int		fd_file;
	char	*error;

	fd_file = open(infile->content, O_RDONLY);
	if (fd_file == -1)
	{
		error = ft_strjoin_modif("minishell: ",
				(char *)infile->content);
		perror(error);
		return (1);
	}
	dup2(fd_file, STDIN_FILENO);
	close(fd_file);
	return (0);
}

int	handle_infile_redirections(t_list_red **tmp_infiles)
{
	while (*tmp_infiles != NULL)
	{
		if ((*tmp_infiles)->doble != 0)
			handle_infile_doble(*tmp_infiles);
		else if (handle_infile_file(*tmp_infiles) != 0)
			return (1);
		*tmp_infiles = (*tmp_infiles)->next;
	}
	return (0);
}
