/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 15:35:41 by fda-roch          #+#    #+#             */
/*   Updated: 2026/02/11 18:35:14 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// 1st step: only executing one command
void	execute_cmds(t_cmdnode *data) // taking command as if absolute path directly
{
	int	i;
	i = 0;
	while (data[i].next != NULL)// remember if phrase has all arguments together, then I need to apply ft_split and then I already have the necessary format for the second argument that execve asks for. char **argv
	{
		printf("cmd %d %s\n", i, data[i].cmd);
		i++;
	}
	/* printf("cmd 1 %s\n", data[0].cmd);
	printf("cmd 2 %s\n", data[1].cmd); */
	/* if (execve(data->cmd, data->phrase, NULL) < 0)
	{
		perror(data->cmd);
		//free(data.cmd); // liberar todo si no funciona!!! Muy importante hacerlo inmediatamente después
		//ft_error_args(args);
		exit(127);
	} */
}

int	main(void)
{
	t_cmdnode	data[3];
	char		*cmd = "/usr/bin/echo";
	char		*cmd2 = "/usr/bin/ls";
	char		*path[] = {"Hola", NULL};
	char		*path2[] = {NULL};
	
	data[0].cmd = cmd;
	data[0].phrase = path;
	data[0].prev = &data[2]; // linking both nodes
	data[0].next = &data[1];
	data[1].prev = &data[0];
	data[1].cmd = cmd2;
	data[1].phrase = path2;
	data[1].next = &data[2];
	data[2].cmd = NULL;
	data[2].next = NULL;
	data[2].prev = &data[1];
	execute_cmds(data);
	return (0);
} 

