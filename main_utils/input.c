/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 08:00:44 by oscar             #+#    #+#             */
/*   Updated: 2026/03/24 13:02:24 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	update_quote_state_line(const char *line, char *quote)
{
	size_t	i;

	i = 0;
	while (line[i])
	{
		if (*quote == 0 && (line[i] == '\'' || line[i] == '"'))
			*quote = line[i];
		else if (*quote && line[i] == *quote)
			*quote = 0;
		i++;
	}
}

char	*join_lines(char *base, const char *extra)
{
	size_t	len1;
	size_t	len2;
	char	*res;

	if (!base)
		return (ft_strdup(extra));
	len1 = ft_strlen(base);
	len2 = ft_strlen(extra);
	res = malloc(len1 + len2 + 2);
	if (!res)
	{
		perror("malloc");
		freedom((void **)&base);
		return (NULL);
	}
	ft_memcpy(res, base, len1);
	res[len1] = '\n';
	ft_memcpy(res + len1 + 1, extra, len2);
	res[len1 + len2 + 1] = '\0';
	freedom((void **)&base);
	return (res);
}

int	read_prompt_line(const char *prompt, char **out)
{
	char	*line;

	line = readline(prompt);
	if (!line)
	{
		*out = NULL;
		return (0);
	}
	*out = line;
	return (1);
}

int	read_line_select(char **line, int has_prev)
{
	if (!has_prev)
		return (read_prompt_line("minishell$ ", line));
	return (read_prompt_line("> ", line));
}

char	*append_line(char *full, char *line, char *quote)
{
	update_quote_state_line(line, quote);
	full = join_lines(full, line);
	freedom((void **)&line);
	return (full);
}
