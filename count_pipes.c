/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   count_pipes.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/09 18:08:44 by fda-roch          #+#    #+#             */
/*   Updated: 2026/02/09 18:27:28 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	count_pipes(t_intvec array)
{
	size_t	i;
	int	counter;
	
	i = 0;
	counter = 0;
	while (i < array.len)
	{
		if (array.data[i] == 5)
			counter++;
		i++;
	}
	return (counter);
}
