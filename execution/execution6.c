/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution2 copy 5.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 20:03:45 by oscar             #+#    #+#             */
/*   Updated: 2026/03/24 20:08:30 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*handle_direct_cmd(t_exec_pack *p, char *data_cmd)
{
	if (access(data_cmd, F_OK) != 0)
		handle_fok_error(p, data_cmd);
	if (access(data_cmd, X_OK) != 0)
		handle_xok_error(p, data_cmd);
	return (data_cmd);
}

char	*get_cmd_name_and_path(t_exec_pack *p,
	char **args, char *data_cmd)
{
	char	*cmd_name;
	char	*cmd_path;

	cmd_path = NULL;
	if (find_chr(data_cmd, '/') == 1)
	{
		cmd_name = strchr_modif(data_cmd, '/');
		cmd_path = handle_direct_cmd(p, data_cmd);
		args[0] = cmd_name;
	}
	else
	{
		cmd_path = get_cmd_path(p, data_cmd);
		args[0] = data_cmd;
	}
	return (cmd_path);
}

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
