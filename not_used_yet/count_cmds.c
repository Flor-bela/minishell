/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   count_cmds.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 13:56:13 by fda-roch          #+#    #+#             */
/*   Updated: 2026/02/09 18:24:54 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	count_cmds(t_intvec array) //counting commands not execution units yet
{
	int	len;
	int	i;
	int	counter_cmd;

	len = array.len;
	i = 0;
	counter_cmd = 0;
	while (i < len)
	{
		if(array.data[i] == 0)
			counter_cmd++;
		i++;
	}
}
