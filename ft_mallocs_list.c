/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_mallocs_list.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/03 18:00:32 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/11 11:31:49 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// quizás tengo que añadir un flag a la lista para hacer free solamente hasta ciertos puntos
static void	ft_free(t_list *mallocs_list)
{
	t_list	*temp;
	
	while (mallocs_list != NULL)
	{
		temp = mallocs_list->next;
		free(mallocs_list->content);
		free(mallocs_list);
		mallocs_list = temp;
	}
}

static void	*ft_malloc_core(size_t size, int flag)
{
	static t_list	*mallocs_list;
	t_list			*new_malloc;
	void			*result;

	result = NULL;
	if (flag == 1)
	{
		result = malloc(size);
		if (result == NULL)
		{
			ft_free(mallocs_list);
			exit(1);
		}
		new_malloc = ft_lstnew(result);
		if (new_malloc == NULL)
		{
			free(result);
			ft_free(mallocs_list);
			exit(
				1);
		}
		ft_lstadd_back(&mallocs_list, new_malloc);
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
