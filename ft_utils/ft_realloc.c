/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_realloc.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/17 12:35:14 by oscar             #+#    #+#             */
/*   Updated: 2026/03/17 13:00:46 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	*ft_realloc(void *ptr, size_t old_size, size_t new_size)
{
	size_t			i;
	unsigned char	*new_ptr;
	unsigned char	*old_ptr;

	if (new_size == 0)
	{
		free(ptr);
		return (NULL);
	}
	new_ptr = (unsigned char *)malloc(new_size);
	if (!new_ptr)
		return (NULL);
	if (!ptr)
		return (new_ptr);
	old_ptr = (unsigned char *)ptr;
	i = 0;
	while (i < old_size && i < new_size)
	{
		new_ptr[i] = old_ptr[i];
		i++;
	}
	free(ptr);
	return (new_ptr);
}
