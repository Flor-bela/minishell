/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <fda-roch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 13:56:13 by fda-roch          #+#    #+#             */
/*   Updated: 2026/02/04 17:05:01 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	check_cmd(t_intvec array)
{
	int	len;
	int	i;
	int	counter_cmd;

	len = array.len;
	i = 0;
	counter_cmd = 0;
	// procurar en data cuantos comandos tengo para ver cuantos procesos hijos crear...algo asi?? pero siendo un array de int como hago para recogerlo?
	while (i < len)
	{
		if(array.data[i] == 0)
			counter_cmd++;
	}
	// aqui supuestamente ya se cuantos tengo y podría empezar a ver paths??? o a crear los child processes??
	
}