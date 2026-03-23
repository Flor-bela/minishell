/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_core.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 08:20:10 by oscar             #+#    #+#             */
/*   Updated: 2026/03/23 08:21:40 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "provisionaryFunctions.h"

static int	process_heredoc_line(t_heredoc_ctx *ctx)
{
	char	*tmp;

	trim_newline(ctx->line);
	if (ft_strcmp(ctx->line, ctx->delimiter) == 0)
	{
		free(ctx->line);
		return (1);
	}
	ctx->line = expand_line_if_needed(ctx->line,
			ctx->expand, ctx->env);
	tmp = append_line_content(ctx->content, ctx->line);
	free(ctx->line);
	if (!tmp)
		return (-1);
	ctx->content = tmp;
	return (0);
}

static int	init_heredoc_ctx(t_heredoc_ctx *ctx,
	const char *delimiter, int expand, char **env)
{
	ctx->content = ft_strdup("");
	if (!ctx->content)
		return (0);
	ctx->delimiter = delimiter;
	ctx->expand = expand;
	ctx->env = env;
	return (1);
}

static int	heredoc_loop(t_heredoc_ctx *ctx)
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
	{
		//free(ctx.content);
		return (NULL);
	}
	free((char *)delimiter);
	return (ctx.content);
}

char	*read_fd_all(int fd)
{
	char	buffer[1025];
	char	*result;
	ssize_t	bytes;

	result = ft_strdup("");
	if (!result)
		return (NULL);
	while (1)
	{
		bytes = read(fd, buffer, 1024);
		if (bytes < 0)
		{
			free(result);
			return (NULL);
		}
		if (bytes == 0)
			break ;
		buffer[bytes] = '\0';
		result = append_buffer(result, buffer);
		if (!result)
			return (NULL);
	}
	return (result);
}
