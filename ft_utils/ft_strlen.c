/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlen.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 15:19:18 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/19 18:17:21 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

size_t	ft_strlen(const char *s)
{
	size_t	num;

	////printf("DEBUG: ft_strlen input=%p\n", s);
	num = 0;
	if (!s)
	{
		//printf("ERROR: ft_strlen NULL\n");
		return (0);
	}
	////printf("DEBUG: ft_strlen input=%p\n", s);
	while (*s != '\0')
	{
		num++;
		s++;
	}
	return (num);
}
