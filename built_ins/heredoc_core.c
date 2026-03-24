/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_core.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 06:10:40 by oscar             #+#    #+#             */
/*   Updated: 2026/03/24 13:02:24 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	check_sigint(char *line, char *content)
{
	if (g_sigint_received)
	{
		g_sigint_received = 0;
		freedom((void **)&line);
		freedom((void **)&content);
		line = NULL;
		content = NULL;
		return (1);
	}
	return (0);
}

void	trim_newline(char *line)
{
	size_t	len;

	len = ft_strlen(line);
	if (len > 0 && line[len - 1] == '\n')
		line[len - 1] = '\0';
}

char	*expand_line_if_needed(char *line, int expand, char **env)
{
	char	*expanded;

	if (!expand)
		return (line);
	expanded = expand_heredoc_line(line, env);
	freedom((void **)&line);
	return (expanded);
}

char	*append_line_content(char *content, char *line)
{
	char	*tmp;

	tmp = join_and_free(content, line);
	if (!tmp)
		return (NULL);
	content = tmp;
	tmp = join_and_free(content, "\n");
	if (!tmp)
		return (NULL);
	return (tmp);
}

int	process_heredoc_line(t_heredoc_ctx *ctx)
{
	char	*tmp;

	trim_newline(ctx->line);
	if (ft_strcmp(ctx->line, ctx->delimiter) == 0)
	{
		freedom((void **)&ctx->line);
		return (1);
	}
	ctx->line = expand_line_if_needed(ctx->line,
			ctx->expand, ctx->env);
	tmp = append_line_content(ctx->content, ctx->line);
	freedom((void **)&ctx->line);
	if (!tmp)
		return (-1);
	ctx->content = tmp;
	return (0);
}
