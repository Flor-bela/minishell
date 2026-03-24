/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_io.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 06:14:57 by oscar             #+#    #+#             */
/*   Updated: 2026/03/24 13:02:24 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*append_buffer(char *result, char *buffer)
{
	char	*tmp;

	tmp = join_and_free(result, buffer);
	if (!tmp)
	{
		freedom((void **)&result);
		return (NULL);
	}
	return (tmp);
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
			freedom((void **)&result);
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
