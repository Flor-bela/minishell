/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strchr_modif.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 12:13:32 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/23 09:16:40 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*strchr_modif(const char *s, int c)
{
	unsigned char	ch;
	int				i;
	int				len;

	ch = (unsigned char)c;
	len = ft_strlen(s);
	i = len - 1;
	while (i > 0)
	{
		if (s[i] == ch)
			return ((char *)&s[i + 1]);
		i--;
	}
	if (ch == '\0')
		return ((char *)&s[i]);
	return (NULL);
}
