/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution2 copy 2.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 20:03:28 by oscar             #+#    #+#             */
/*   Updated: 2026/03/24 20:11:52 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	execute_only_built_in(t_builtin_ctx c)
{
	int	saved_stdin;
	int	saved_stdout;
	int	return_value;

	saved_stdin = -1;
	saved_stdout = -1;
	return_value = 0;
	saved_stdin = dup(STDIN_FILENO);
	return_value = handle_redirects_builtin(&c, &saved_stdout);
	return_value = execute_builtin_if_needed(&c, return_value);
	restore_fds(saved_stdin, saved_stdout);
	return (return_value);
}

int	handle_single_cmd_parent(t_cmdnode *data_tmp,
		t_cmdnode **data, char ***envp, t_exit_ctx *ctx)
{
	int	return_value;

	return_value = 0;
	if ((data_tmp->cmd == NULL || data_tmp->cmd[0] == '\0'))
		return (execute_only_redirect(data_tmp->infiles, data_tmp->outfiles));
	if ((is_built_in(data_tmp->cmd) == 1))
	{
		return_value = execute_only_built_in((t_builtin_ctx){
				data_tmp->infiles,
				data_tmp->outfiles,
				data,
				envp,
				ctx
			});
		return (return_value);
	}
	return (0);
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
