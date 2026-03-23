/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipes.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 08:59:47 by oscar             #+#    #+#             */
/*   Updated: 2026/03/23 10:08:09 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
#include "../parse/parser.h"

int	init_len_and_pipes(t_exec_pack *p)
{
	int			i;
	t_cmdnode	*tmp;

	p->len = 0;
	i = 0;
	tmp = *p->data;
	while (tmp != NULL)
	{
		p->len++;
		tmp = tmp->next;
	}
	p->arr_fds = NULL;
	if (p->len > 1)
	{
		p->arr_fds = (int **)ft_malloc(sizeof(int *) * (p->len - 1));
		while (i < p->len - 1)
		{
			p->arr_fds[i] = (int *)ft_malloc(sizeof(int) * 2);
			if (pipe(p->arr_fds[i]) == -1)
				error_pipe2(p);
			i++;
		}
	}
	return (p->len);
}
