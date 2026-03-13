/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 14:41:33 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/13 11:46:36 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	unsigned char	*us1;
	unsigned char	*us2;
	size_t			i;

	i = 0;
	us1 = (unsigned char *)s1;
	us2 = (unsigned char *)s2;
	while (n > 0 && us1[i] != '\0' && us2[i] != '\0')
	{
		if (us1[i] == us2[i])
			i++;
		else
			return (us1[i] - us2[i]);
		n--;
	}
	if (n > 0)
		return (us1[i] - us2[i]);
	return (0);
}
