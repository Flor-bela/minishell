/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 15:35:41 by fda-roch          #+#    #+#             */
/*   Updated: 2026/02/11 19:26:13 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	error_fork(void)
{
	perror("fork"); // fork is what is written in the manual for fork
	// free something more here?
	exit(EXIT_FAILURE);
}

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
	// Uso lo de antes para contar cuantos nodos tiene la estructura.
	pid_t	*pids; // array de pids
	int	len;

	len = i;
	i = 0;
	pids = malloc(sizeof(len));
	/* if (pids == NULL)
		return (1); */
	while (i < len) // ir executando este loop para crear los procesos y ejecutar los comandos
	{
		pids[i] = fork();
		if (pids[i] == -1)
			error_fork();
		if (pids[i] == 0)
		{
			// funcion executa comando
			printf("Executando comando %d\n", i);
			execve(data[i].cmd, data[i].phrase, NULL);
			// error si no executa la funcion con el exit code correcto
		}
		i++;
	}
	/* if (execve(data->cmd, data->phrase, NULL) < 0)
	{
		perror(data->cmd);
		//free(data.cmd); // liberar todo si no funciona!!! Muy importante hacerlo inmediatamente después
		//ft_error_args(args);
		exit(127);
	} */
i = 0;
int status;

status = 0; // not sure about this...
while (i < len)
{
	waitpid(pids[i], &status, 0); // ?? deberia estar fuera del loop? creo que sí
	i++;
}
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

