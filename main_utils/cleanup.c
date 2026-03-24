/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 07:58:42 by oscar             #+#    #+#             */
/*   Updated: 2026/03/24 13:02:24 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	intvec_destroy(t_intvec *v)
{
	freedom((void **)&v->data);
	v->data = NULL;
	v->len = 0;
	v->cap = 0;
}

void	str_list_destroy(t_list *lst)
{
	t_list	*next;

	while (lst)
	{
		next = lst->next;
		freedom((void **)&lst->content);
		freedom((void **)&lst);
		lst = next;
	}
}

void	red_list_destroy(t_list_red *lst)
{
	t_list_red	*next;

	while (lst)
	{
		next = lst->next;
		freedom((void **)&lst->content);
		freedom((void **)&lst);
		lst = next;
	}
}

void	cmd_list_destroy(t_cmdnode *h)
{
	t_cmdnode	*nx;

	while (h)
	{
		nx = h->next;
		freedom((void **)&h->cmd);
		str_list_destroy(h->phrase);
		red_list_destroy(h->infiles);
		red_list_destroy(h->outfiles);
		freedom((void **)&h);
		h = nx;
	}
}
