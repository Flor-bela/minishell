/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   close_fds.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <fda-roch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 17:56:01 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/13 14:33:36 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	close_fds(int **arr_fds, int len_cmds)
{
	int	j;

	j = 0;
	while (j < len_cmds - 1)
	{
		close(arr_fds[j][0]);
		close(arr_fds[j][1]);
		j++;
	}
}