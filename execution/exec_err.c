/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_err.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 23:12:49 by fda-roch          #+#    #+#             */
/*   Updated: 2026/10/14 23:36:46 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	error_execve(char **envp, char *cmd_path)
{
	char	*error;

	error = ft_strjoin_modif("minishell: ", cmd_path);
	perror(error);
	if (errno == EACCES)
	{
		change_exit_value("126", &envp);
		exit(126);
	}
	else if (errno == ENOENT)
	{
		change_exit_value("127", &envp);
		exit(127);
	}
	else
	{
		change_exit_value("126", &envp);
		exit(126);
	}
}

void	exit_error(t_exec_pack *p, char *code, int exit_code)
{
	change_exit_value(code, &p->envp);
	free_envp_copy(p->envp);
	cmd_list_destroy(*p->data);
	free(p->line);
	intvec_destroy(p->codes);
	exit(exit_code);
}

void	handle_fok_error(t_exec_pack *p, char *data_cmd)
{
	char	*error;

	error = ft_strjoin_modif("minishell: ", data_cmd);
	perror(error);
	close_fds(p->arr_fds, p->len);
	ft_free_malloc();
	if (errno == ENOTDIR)
		exit_error(p, "126", 126);
	else if (errno == ENOENT)
		exit_error(p, "127", 127);
}

void	handle_xok_error(t_exec_pack *p, char *data_cmd)
{
	char	*error;

	error = ft_strjoin_modif("minishell: ", data_cmd);
	perror(error);
	change_exit_value("126", &p->envp);
	close_fds(p->arr_fds, p->len);
	free_envp_copy(p->envp);
	cmd_list_destroy(*p->data);
	free(p->line);
	intvec_destroy(p->codes);
	exit(126);
}

void	handle_exec_error(t_exec_pipe_ctx *c, int return_value)
{
	if (return_value != 0 || !c->data_tmp->cmd)
	{
		clean_execution(c->p->envp, c->p->data,
			c->p->line, c->p->codes);
		exit(return_value);
	}
}
