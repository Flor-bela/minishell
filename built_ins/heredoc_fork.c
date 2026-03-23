/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_fork.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 08:21:53 by oscar             #+#    #+#             */
/*   Updated: 2026/03/23 10:18:20 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "provisionaryFunctions.h"

static void	heredoc_child(t_heredoc_fork *ctx, int fd[2], char *target)
{
	char	*content;

	close(fd[0]);
	content = read_heredoc_input(ctx->delimiter, ctx->expand, ctx->env);
	if (!content)
	{
		close(fd[1]);
		free(target);
		ft_free_malloc();
		free_envp_copy(ctx->env);
		cmd_list_destroy(*ctx->data);
		free(ctx->line);
		intvec_destroy(ctx->codes);
		exit(130);
	}
	free(target);
	write(fd[1], content, ft_strlen(content));
	free(content);
	close(fd[1]);
	ft_free_malloc();
	free_envp_copy(ctx->env);
	cmd_list_destroy(*ctx->data);
	free(ctx->line);
	intvec_destroy(ctx->codes);
	exit(0);
}

static int	check_heredoc_signal(int status, char ***env, int fd)
{
	if (g_sigint_received == 1
		|| (WIFEXITED(status) && WEXITSTATUS(status) == 130)
		|| (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT))
	{
		close(fd);
		g_sigint_received = 1;
		change_exit_value("130", env);
		return (1);
	}
	return (0);
}

static void free_red_list(t_list_red *h)
{
	t_list_red *nx;

	while (h)
	{
		nx = h->next;
		if (h->content)
			free(h->content);
		free(h);
		h = nx;
	}
}

char *heredoc_with_fork(t_heredoc_fork *ctx, t_heredoc_args *args)
{
	int		fd[2];
	pid_t	pid;
	char	*content;
	int		status;

	if (pipe(fd) == -1)
		return (NULL);
	pid = fork();
	if (pid == -1)
		return (NULL);
	if (pid == 0)
	{
		////
		if (args && args->parser)
		{
			free_red_list(args->parser->pending_in); // esto libera todo cuando hacemos << here << abc << lol, pero puede que de problema cuando este en un pipeline....
			free_red_list(args->parser->pending_out);
			//if (args->parser->head)
				//cmd_list_destroy(args->parser->head); // creo que no hace falta???
		}
		//ctx->delimiter if args is NULL:
		char *target_str;
		if (args != NULL)
			target_str = (char *)args->target;
		else
			target_str = NULL;
		//
		heredoc_child(ctx, fd, target_str);
	}
	close(fd[1]);
	waitpid(pid, &status, 0);
	if (check_heredoc_signal(status, &ctx->env, fd[0]))
		return (NULL);
	content = read_fd_all(fd[0]);
	close(fd[0]);
	return (content);
}

char	*append_buffer(char *result, char *buffer)
{
	char	*tmp;

	tmp = join_and_free(result, buffer);
	if (!tmp)
	{
		free(result);
		return (NULL);
	}
	return (tmp);
}
