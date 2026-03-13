/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strchr_modif.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 12:13:32 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/13 14:04:41 by fda-roch         ###   ########.fr       */
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
	i = len - 1; // empieza justo después del caracter nulo
	while (i > 0)
	{
		if (s[i] == ch)
			return ((char *)&s[i + 1]); // +1 para que no me devuelva el / pero justo lo que está a la derecha, o sea el nombre de comando en sí
		i--; // así va desde el final hasta el inicio de la string
	}
	if (ch == '\0')
		return ((char *)&s[i]);
	return (NULL);
}
