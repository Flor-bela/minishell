/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <fda-roch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 15:35:41 by fda-roch          #+#    #+#             */
/*   Updated: 2026/02/10 17:37:37 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// 1st step: only executing one command -> todo: loop it and create new processes for each command!
void	execute_cmds(t_cmdnode data) // taking command as if absolute path directly
{
	// remember if phrase has all arguments together, then I need to apply ft_split and then I already have the necessary format for the second argument that execve asks for. char **argv
	printf("Hola\n");
	if (execve(data.cmd, data.phrase, NULL) < 0)
	{
		perror(data.cmd);
		//free(data.cmd); // liberar todo si no funciona!!! Muy importante hacerlo inmediatamente después
		//ft_error_args(args);
		exit(127);
	}
}

int	main(void)
{
	t_cmdnode	data[2];
	char		*cmd = "/usr/bin/echo";
	char		*cmd2 = "/usr/bin/ls";
	char		*path[] = {"Hola", NULL};
	char		*path2[] = {NULL};
	
	data[0].cmd = cmd;
	data[0].phrase = path;
	data[0].prev = NULL;
	data[1].cmd = cmd2;
	data[1].phrase = path2;
	data[1].next = NULL;
	execute_cmds(data[0]);
	return (0);
} 

