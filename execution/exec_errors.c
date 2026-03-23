/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_errors.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 08:40:26 by oscar             #+#    #+#             */
/*   Updated: 2026/03/23 09:54:37 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	exit_error(t_exec_pack *p, char *code, int exit_code)
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

void	handle_heredoc_error(t_exec_pack *p)
{
	change_exit_value("130", &p->envp);
	ft_free_malloc();
	free_envp_copy(p->envp);
	cmd_list_destroy(*p->data);
	free(p->line);
	intvec_destroy(p->codes);
	exit(130);
}

void	error_fd_file(char *name_infile, t_exec_pack *p)
{
	char	*error;

	error = ft_strjoin_modif("minishell: ", name_infile);
	perror(error);
	ft_free_malloc();
	change_exit_value("1", &p->envp);
	free_envp_copy(p->envp);
	cmd_list_destroy(*p->data);
	free(p->line);
	intvec_destroy(p->codes);
	exit(1);
}
