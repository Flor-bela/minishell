/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 08:02:40 by oscar             #+#    #+#             */
/*   Updated: 2026/03/24 13:02:24 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*read_full_command(void)
{
	char	*line;
	char	*full;
	char	quote;
	int		status;

	full = NULL;
	quote = 0;
	while (1)
	{
		status = read_line_select(&line, full != NULL);
		if (status <= 0)
			return (full);
		full = append_line(full, line, &quote);
		if (!full)
			return (NULL);
		if (quote == 0)
			break ;
	}
	return (full);
}

int	is_only_spaces(const char *s)
{
	size_t	i;

	i = 0;
	while (s[i])
	{
		if (!isspace((unsigned char)s[i]))
			return (0);
		i++;
	}
	return (1);
}

int	process_input(char **line)
{
	*line = read_full_command();
	if (!*line)
	{
		printf("exit\n");
		return (1);
	}
	if ((*line)[0] == '\0' || is_only_spaces(*line))
	{
		freedom((void **)&*line);
		return (2);
	}
	add_history(*line);
	return (0);
}
