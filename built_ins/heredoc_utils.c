/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 08:19:00 by oscar             #+#    #+#             */
/*   Updated: 2026/03/23 10:18:24 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "provisionaryFunctions.h"

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
		free(buf);
		return (NULL);
	}
	buf[i] = '\0';
	return (buf);
}

int	check_sigint(char *line, char *content)
{
	if (g_sigint_received)
	{
		g_sigint_received = 0;
		free(line);
		free(content);
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
	free(line);
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
