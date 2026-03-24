/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   close_fds.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 17:56:01 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/24 13:06:09 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	close_fds(int **arr_fds, int len_cmds)
{
	int	j;

	j = 0;
	if (arr_fds == NULL)
		return ;
	while (j < len_cmds - 1 && arr_fds != NULL)
	{
		close(arr_fds[j][0]);
		close(arr_fds[j][1]);
		j++;
	}
}

void freedom(void** ptr)
{
	if(ptr && *ptr)
	{
		free(*ptr);
		*ptr=NULL;
	}
}
