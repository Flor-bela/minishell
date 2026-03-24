/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution2 copy 4.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 20:03:41 by oscar             #+#    #+#             */
/*   Updated: 2026/03/24 20:09:22 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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

int	check_if_single_cmd_parent(char *data_tmp_cmd)
{
	if (data_tmp_cmd == NULL || data_tmp_cmd[0] == '\0')
		return (1);
	if (is_built_in(data_tmp_cmd) == 1)
		return (1);
	return (0);
}
