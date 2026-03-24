/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_fork.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 06:16:08 by oscar             #+#    #+#             */
/*   Updated: 2026/03/24 13:02:24 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	heredoc_cleanup_exit(t_heredoc_fork *ctx, int exit_code)
{
	freedom((void **)&ctx->delimiter);
	ft_free_malloc();
	free_envp_copy(ctx->env);
	cmd_list_destroy(*ctx->data);
	freedom((void **)&ctx->line);
	intvec_destroy(ctx->codes);
	exit(exit_code);
}

void	heredoc_child(t_heredoc_fork *ctx, int fd[2], char *target)
{
	char	*content;

	close(fd[0]);
	content = read_heredoc_input(ctx->delimiter,
			ctx->expand, ctx->env);
	if (!content)
	{
		close(fd[1]);
		freedom((void **)&target);
		heredoc_cleanup_exit(ctx, 130);
	}
	freedom((void **)&target);
	write(fd[1], content, ft_strlen(content));
	freedom((void **)&content);
	close(fd[1]);
	ft_free_malloc();
	free_envp_copy(ctx->env);
	cmd_list_destroy(*ctx->data);
	freedom((void **)&ctx->line);
	intvec_destroy(ctx->codes);
	exit(0);
}

int	check_heredoc_signal(int status, char ***env, int fd)
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

void	free_red_list(t_list_red *h)
{
	t_list_red	*nx;

	while (h)
	{
		nx = h->next;
		if (h->content)
			freedom((void **)&h->content);
		freedom((void **)&h);
		h = nx;
	}
}

void	heredoc_child_process(t_heredoc_fork *ctx,
		t_heredoc_args *args, t_cmdnode *cmd, int fd[2])
{
	char	*target_str;

	cmd_list_destroy(cmd);
	if (args && args->parser)
	{
		free_red_list(args->parser->pending_in);
		free_red_list(args->parser->pending_out);
	}
	if (args != NULL)
		target_str = (char *)args->target;
	else
		target_str = NULL;
	heredoc_child(ctx, fd, target_str);
}
