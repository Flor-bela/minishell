/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtin.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 23:08:19 by fda-roch          #+#    #+#             */
/*   Updated: 2026/10/14 23:37:04 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	execute_built_ins(t_cmdnode *data_tmp,
		char ***envp, t_exit_ctx *ctx)
{
	int	return_value;

	return_value = 0;
	if (data_tmp->cmd && (ft_strncmp(data_tmp->cmd,
				"echo", 4) == 0 && data_tmp->cmd[4] == '\0'))
		return_value = builtin_echo(data_tmp->phrase);
	else if (data_tmp->cmd && (ft_strncmp(data_tmp->cmd,
				"cd", 2) == 0 && data_tmp->cmd[2] == '\0'))
		return_value = builtin_cd(data_tmp->phrase, *envp);
	else if (data_tmp->cmd && (ft_strncmp(data_tmp->cmd,
				"pwd", 3) == 0 && data_tmp->cmd[3] == '\0'))
		return_value = builtin_pwd();
	else if (data_tmp->cmd && (ft_strncmp(data_tmp->cmd,
				"export", 6) == 0 && data_tmp->cmd[6] == '\0'))
		return_value = builtin_export(data_tmp->phrase, envp);
	else if (data_tmp->cmd && (ft_strncmp(data_tmp->cmd,
				"unset", 5) == 0 && data_tmp->cmd[5] == '\0'))
		return_value = builtin_unset(data_tmp->phrase, envp);
	else if (data_tmp->cmd && (ft_strncmp(data_tmp->cmd,
				"env", 3) == 0 && data_tmp->cmd[3] == '\0'))
		return_value = builtin_env(*envp);
	else if (data_tmp->cmd && (ft_strncmp(data_tmp->cmd,
				"exit", 4) == 0 && data_tmp->cmd[4] == '\0'))
		return_value = builtin_exit(ctx);
	return (return_value);
}

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

int	check_if_single_cmd_parent(char *data_tmp_cmd)
{
	if (data_tmp_cmd == NULL || data_tmp_cmd[0] == '\0')
		return (1);
	if (is_built_in(data_tmp_cmd) == 1)
		return (1);
	return (0);
}
