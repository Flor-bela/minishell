/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   close_fds.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 17:56:01 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/13 14:02:30 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	close_fds(int **arr_fds, int len_cmds)
{
	int	j;

	j = 0;
	while (j < len_cmds - 1) // close all fds from all pipes created (also here in the parent process)
	{
		close(arr_fds[j][0]); //  to read
		close(arr_fds[j][1]); // cierro ya?? (creo)
		j++;
	}
}