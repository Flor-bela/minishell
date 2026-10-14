/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipeline.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 23:06:11 by fda-roch          #+#    #+#             */
/*   Updated: 2026/10/14 23:08:00 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	**execute_pipe(t_cmdnode **data,
		t_intvec **codes, char *line, char **envp)
{
	int			**arr_fds;
	int			len;
	int			i;

	i = 0;
	len = len_data(*data);
	arr_fds = (int **)ft_malloc(sizeof(int *) * (len - 1));
	while (i < len - 1)
	{
		arr_fds[i] = (int *)ft_malloc(sizeof(int) * 2);
		if (pipe(arr_fds[i]) == -1)
			error_pipe(data, codes, line, envp);
		i++;
	}
	return (arr_fds);
}

int	execute_red_first_cmd(t_list_red *data_infiles,
		t_list_red *data_outfiles, int len, int **arr_fds)
{
	int	return_value;

	return_value = 0;
	if (data_infiles != NULL)
		return_value = handle_infile_redirections(&data_infiles);
	if (len > 1 && data_outfiles == NULL)
		dup2(arr_fds[0][1], STDOUT_FILENO);
	if (data_outfiles != NULL)
		return_value = handle_outfile_redirections(&data_outfiles);
	return (return_value);
}

int	execute_red_last_cmd(t_list_red *data_infiles,
		t_list_red *data_outfiles, int **arr_fds, int i)
{
	int	return_value;

	return_value = 0;
	if (data_infiles != NULL)
		return_value = handle_infile_redirections(&data_infiles);
	else
		dup2(arr_fds[i - 1][0], STDIN_FILENO);
	if (data_outfiles != NULL)
		return_value = handle_outfile_redirections(&data_outfiles);
	return (return_value);
}

int	execute_other_cmds(t_list_red *data_infiles,
		t_list_red *data_outfiles, int **arr_fds, int i)
{
	int	return_value;

	return_value = 0;
	if (data_infiles != NULL)
		return_value = handle_infile_redirections(&data_infiles);
	else
		dup2(arr_fds[i - 1][0], STDIN_FILENO);
	if (data_outfiles != NULL)
		return_value = handle_outfile_redirections(&data_outfiles);
	else
		dup2(arr_fds[i][1], STDOUT_FILENO);
	return (return_value);
}
