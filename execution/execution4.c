/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution2 copy 3.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 20:03:36 by oscar             #+#    #+#             */
/*   Updated: 2026/03/24 20:10:34 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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

int	handle_redirects_builtin(t_builtin_ctx *c,
	int *saved_stdout)
{
	int	return_value;

	return_value = 0;
	if (c->data_infiles != NULL)
		return_value = handle_infile_redirections(&c->data_infiles);
	if (c->data_outfiles != NULL)
	{
		*saved_stdout = dup(STDOUT_FILENO);
		return_value = handle_outfile_redirections(&c->data_outfiles);
	}
	return (return_value);
}

int	execute_builtin_if_needed(t_builtin_ctx *c,
	int return_value)
{
	if (is_built_in((*c->data_tmp)->cmd) == 1)
		return_value = execute_built_ins(*c->data_tmp,
				c->envp, c->ctx);
	return (return_value);
}
