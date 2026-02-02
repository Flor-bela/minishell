/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <fda-roch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 13:56:13 by fda-roch          #+#    #+#             */
/*   Updated: 2026/02/02 13:58:56 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// pseudocode

int	eval(t_node node)
{
	if (node.type == PLUS)
		return (node.left + node.right);
	else
		return (node.value);
}

int	execute(t_node node)
{
	if (node.type == PIPE)
		return (execute_pipe(node.left, node.right));
	else
		return (execute_simple_command(node.value));
}


