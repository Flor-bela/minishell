/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkalgano <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 06:00:32 by oscar             #+#    #+#             */
/*   Updated: 2026/03/24 16:17:27 by dkalgano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*get_next_line(int fd)
{
	char	*buf;
	int		i;
	char	c;

	buf = malloc(1024);
	i = 0;
	if (!buf)
		return (NULL);
	while (read(fd, &c, 1) > 0)
	{
		buf[i++] = c;
		if (c == '\n')
			break ;
	}
	if (i == 0)
	{
		freedom((void **)&buf);
		return (NULL);
	}
	buf[i] = '\0';
	return (buf);
}

int	heredoc_loop(t_heredoc_ctx *ctx)
{
	while (1)
	{
		write(1, "> ", 2);
		ctx->line = get_next_line(STDIN_FILENO);
		if (check_sigint(ctx->line, ctx->content))
			return (0);
		if (!ctx->line)
			break ;
		ctx->status = process_heredoc_line(ctx);
		if (ctx->status == 1)
			break ;
		if (ctx->status == -1)
			return (0);
	}
	return (1);
}

char	*read_heredoc_input(const char *delimiter,
	int expand_vars, char **shell_env)
{
	t_heredoc_ctx	ctx;

	g_sigint_received = 0;
	setup_signals_heredoc();
	if (!init_heredoc_ctx(&ctx, delimiter, expand_vars, shell_env))
		return (NULL);
	if (!heredoc_loop(&ctx))
		return (NULL);
	freedom((void **)&delimiter);
	return (ctx.content);
}

int	heredoc_with_fork(t_heredoc_fork *ctx,
		t_heredoc_args *args)
{
	int		fd[2];
	pid_t	pid;
	int		status;

	if (pipe(fd) == -1)
		return (-1);
	pid = fork();
	if (pid == -1)
		return (-1);
	if (pid == 0)
		heredoc_child_process(ctx, args, fd);
	close(fd[1]);
	waitpid(pid, &status, 0);
	if (check_heredoc_signal(status, &ctx->env, fd[0]))
		return (-1);
	return (fd[0]);
}
