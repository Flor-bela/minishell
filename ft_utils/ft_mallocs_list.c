/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_mallocs_list.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/03 18:00:32 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/24 13:02:24 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	ft_free(t_list *mallocs_list)
{
	t_list	*temp;

	while (mallocs_list != NULL)
	{
		temp = mallocs_list->next;
		freedom((void **)&mallocs_list->content);
		freedom((void **)&mallocs_list);
		mallocs_list = temp;
	}
}

static void	*register_malloc(void *ptr, t_list **lst)
{
	t_list	*node;

	node = ft_lstnew(ptr);
	if (!node)
	{
		freedom((void **)&ptr);
		ft_free(*lst);
		exit(1);
	}
	ft_lstadd_back(lst, node);
	return (ptr);
}

static void	*ft_malloc_core(size_t size, int flag)
{
	static t_list	*mallocs_list;
	void			*result;

	result = NULL;
	if (flag == 1)
	{
		result = malloc(size);
		if (!result)
		{
			ft_free(mallocs_list);
			exit(1);
		}
		result = register_malloc(result, &mallocs_list);
	}
	else if (flag == 2)
		ft_free(mallocs_list);
	return (result);
}

void	*ft_malloc(size_t size)
{
	return (ft_malloc_core(size, 1));
}

void	ft_free_malloc(void)
{
	ft_malloc_core(0, 2);
}
