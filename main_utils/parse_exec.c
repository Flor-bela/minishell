/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_exec.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkalgano <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 08:04:08 by oscar             #+#    #+#             */
/*   Updated: 2026/03/24 18:18:19 by dkalgano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	intvec_reset(t_intvec *v)
{
	v->data = NULL;
	v->len = 0;
	v->cap = 0;
}

int	handle_parse(char *line, t_intvec *codes,
	t_cmdnode **list, char ***env)
{
	int	exit_code;

	intvec_reset(codes);
	*list = NULL;
	if (parse_line(line, codes, list, *env) == 0)
	{
		exit_code = execute_cmds(list, env, line, codes);
		char *exit_str = ft_itoa(exit_code);
		change_exit_value(exit_str, env);
		free(exit_str);
		return (0);
	}
	if (!g_sigint_received)
		change_exit_value("2", env);
	g_sigint_received = 0;
	return (1);

}

void	cleanup_iteration(t_cmdnode *list,
	t_intvec *codes, char *line)
{
	cmd_list_destroy(list);
	intvec_destroy(codes);
	freedom((void **)&line);
}
