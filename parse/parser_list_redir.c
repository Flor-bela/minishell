/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_list_redir.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 10:13:25 by oscar             #+#    #+#             */
/*   Updated: 2026/03/19 16:09:53 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

void	str_list_free(t_list *lst)
{
	t_list	*next;

	while (lst)
	{
		next = lst->next;
		free(lst->content);
		free(lst);
		lst = next;
	}
}

static t_list_red	*red_node_new(const char *s, int doble)
{
	t_list_red	*node;

	node = malloc(sizeof(t_list_red));
	if (!node)
	{
		perror("malloc");
		exit(1);
	}
	if (s)
		node->content = ft_strdup(s);
	else
		node->content = NULL;
	if (s && !node->content)
	{
		perror("ft_strdup");
		exit(1);
	}
	node->doble = doble;
	node->prev = NULL;
	node->next = NULL;
	node->is_heredoc = 0;
	return (node);
}

static void	red_list_append(t_list_red **head, t_list_red **tail,
	const char *s, int doble)
{
	t_list_red	*node;

	node = red_node_new(s, doble);
	if (!*head)
	{
		*head = node;
		*tail = node;
	}
	else
	{
		node->prev = *tail;
		(*tail)->next = node;
		*tail = node;
	}
}

t_list_red *red_list_push_back(t_list_red **head, const char *s, int doble)
{
	t_list_red *tail;
	t_list_red *new;

	tail = *head;
	if (tail)
	{
		while (tail->next)
			tail = tail->next;
	}

	new = red_node_new(s, doble);

	if (!*head)
		*head = new;
	else
	{
		tail->next = new;
		new->prev = tail;
	}

	return (new); // 🔥 IMPORTANTE
}

void	red_list_free(t_list_red *lst)
{
	t_list_red	*next;

	while (lst)
	{
		next = lst->next;
		free(lst->content);
		free(lst);
		lst = next;
	}
}