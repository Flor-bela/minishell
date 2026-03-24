/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell_main.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 23:52:06 by oscar             #+#    #+#             */
/*   Updated: 2026/03/24 09:56:20 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_sigint_received = 0;

static void	main_loop(char ***shell_env)
{
	char		*line;
	t_intvec	codes;
	t_cmdnode	*list;
	int			status;

	while (1)
	{
		status = process_input(&line);
		if (status == 1)
			break ;
		if (status == 2)
			continue ;
		handle_parse(line, &codes, &list, shell_env);
		cleanup_iteration(list, &codes, line);
	}
}

int	main(int argc, char **argv, char **envp)
{
	char	**shell_env;

	(void)argc;
	(void)argv;
	shell_env = dup_envp(envp);
	if (!shell_env)
		return (1);
	setup_signals_prompt();
	using_history();
	change_exit_value("127", &shell_env);
	main_loop(&shell_env);
	rl_clear_history();
	ft_free_malloc();
	free_envp_copy(shell_env);
	return (0);
}
