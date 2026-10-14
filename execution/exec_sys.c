/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_sys.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 23:22:08 by fda-roch          #+#    #+#             */
/*   Updated: 2026/10/14 23:36:02 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	error_fork(t_exec_pack *p)
{
	perror("minishell: fork");
	change_exit_value("1", &p->envp);
	ft_free_malloc();
	free_envp_copy(p->envp);
	cmd_list_destroy(*p->data);
	free(p->line);
	intvec_destroy(p->codes);
	exit(1);
}

void	error_pipe(t_cmdnode **data,
		t_intvec **codes, char *line, char **envp)
{
	perror("minishell: pipe");
	change_exit_value("1", &envp);
	free_envp_copy(envp);
	cmd_list_destroy(*data);
	free(line);
	intvec_destroy(*codes);
	exit(1);
}

int	wait_and_signals(int len, pid_t *pids,
		int last_exit_status, char **envp)
{
	int	signal_code;
	int	i;

	signal_code = 0;
	i = 0;
	while (i < len)
	{
		waitpid(pids[i], &last_exit_status, 0);
		i++;
	}
	signal(SIGINT, handle_sigint);
	signal(SIGQUIT, SIG_IGN);
	if (WIFEXITED(last_exit_status))
		last_exit_status = WEXITSTATUS(last_exit_status);
	else if (WIFSIGNALED(last_exit_status))
	{
		signal_code = WTERMSIG(last_exit_status);
		if (signal_code == SIGINT)
			write(1, "", 0);
		else if (signal_code == SIGQUIT)
			write(1, "Quit (core dumped)\n", 20);
		last_exit_status = signal_code + 128;
	}
	change_exit_value(ft_itoa(last_exit_status), &envp);
	return (last_exit_status);
}
