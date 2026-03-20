/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 12:32:00 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/20 13:42:14 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*ft_strdup(const char *s)
{
	unsigned char	*ch;
	size_t			size;
	size_t			i;
	unsigned char	*ptr;

	size = 0;
	i = 0;
	ch = (unsigned char *)s;
	while (ch[i] != '\0')
		i++;
	size = i + 1;
	ptr = malloc(size);
	if (ptr == NULL)
		return (NULL);
	i = 0;
	while (i < size)
	{
		ptr[i] = ch[i];
		i++;
	}
	return ((char *)ptr);
}
