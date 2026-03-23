/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_forks.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 08:47:33 by oscar             #+#    #+#             */
/*   Updated: 2026/03/23 10:27:05 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	exec_forks(t_exec_pack *p)
{
	pid_t		*pids;
	int			i;
	int			status;
	t_cmdnode	*tmp;

	pids = ft_malloc(sizeof(pid_t) * p->len);
	tmp = *p->data;
	i = 0;
	signal(SIGINT, handle_sigint);
	signal(SIGQUIT, SIG_IGN);
	while (i < p->len)
	{
		pids[i] = fork();
		if (pids[i] == 0)
			child_exec(p, tmp, i);
		tmp = tmp->next;
		i++;
	}
	close_fds(p->arr_fds, p->len);
	i = 0;
	while (i < p->len)
		waitpid(pids[i++], &status, 0);
	return (status);
}
