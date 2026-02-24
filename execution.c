/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <fda-roch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 15:35:41 by fda-roch          #+#    #+#             */
/*   Updated: 2026/02/24 13:51:16 by fda-roch         ###   ########.fr       */
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
int	execute_cmds(t_cmdnode *data) // taking command as its absolute path directly
{
	int	i;
	i = 0;
	while (data[i].next != NULL)
	{
		printf("cmd %d %s\n", i, data[i].cmd);
		i++;
	}
	// Uso lo de antes para contar cuantos nodos tiene la estructura.
	pid_t	*pids; // array de pids
	int		len;

	len = i;
	i = 0;
	pids = ft_malloc(sizeof(pid_t) * len);
	while (i < len) // ir executando este loop para crear los procesos y ejecutar los comandos
	{
		pids[i] = fork();
		if (pids[i] == -1)
			error_fork();
		else if (pids[i] == 0) // in child process
		{
			printf("Executing command %d\n", i);
			// find path if needed here!
			
			// if (data[i].cmd has a /, then it will be the direct pathname)
			char **split;
			int	word_count;
			word_count = 0;
			split = ft_split(data[i].phrase, ' ');
			while (split[word_count] != NULL)
				word_count++;

			char **args;
			args = (char **)ft_malloc(sizeof(char *) * (word_count + 2)); // + 2 para añadir el array con nombre del comando (args[0]) y el ultimo array que es el del caracter nulo
			args[word_count] = NULL;
			
				
			i = 0;
			// here obtain the name of the command:
			if (find_chr(data[i].cmd, '/') == 1) // if it has / in the name of the command...
			{
				char	*cmd_name;
				cmd_name = strchr_modif(data[i].cmd, '/');
				printf("Command name is %s\n", cmd_name);
				// go until the end and obtain the name of the command... from null character to the first / we find...
				// ft_strlen -> veo cuantos tiene
				// despues uso el original ft_strchr desde el final (desde el resultado de ft_strlen) y hago i-- hasta encontrar el ultimo /
				// y ahí obtengo la posicion de la primera letra del comando.
				args[0] = cmd_name;
			}
			else //-> si no tiene / entonces pon el nombre data[i].cmd directamente
				args[0] = data[i].cmd;

			
			while (i < word_count)
			{
				args[i + 1] = split[i];
				i++;
			}
			args[word_count + 1] = NULL;
			
			i = 0;
			if (execve(data[i].cmd, args, NULL) < 0)
			{
				perror(data[i].cmd);
				//free(data.cmd); // liberar todo si no funciona!!! Muy importante hacerlo inmediatamente después
				//ft_error_args(args);
				exit(127); // I think it is 126... check this!!!
			}
		}
		i++;
	}
	i = 0;
	int status;
	status = 0;
	while (i < len)
	{
		waitpid(pids[i], &status, 0);
		i++;
	}
	return (status);
}

int	main(void)
{
	t_cmdnode	data[4];
	char		*cmd = "/usr/bin/echo";
	char		*cmd2 = "echo";
	//char		*cmd3 = "/usr/bin/ls";
	char		path[] = "Hola Mundo"; //-> it is not printing the first word...
	char		path2[] = "World Wide Web";
	//char		path3[] = "-a -a"; // here also only executes if I put the flag twice...doesn´t take the first one like in echo...why??? 
	
	data[0].cmd = cmd;
	data[0].phrase = path;
	data[0].prev = &data[3]; // linking both nodes
	data[0].next = &data[1];
	data[1].prev = &data[0];
	data[1].cmd = cmd2;
	data[1].phrase = path2;
	data[1].next = &data[2];
	data[2].cmd = NULL;
	data[2].phrase = NULL;
	data[2].next = NULL;
	data[2].prev = &data[1];
	/* data[2].cmd = cmd3;
	data[2].phrase = path3;
	data[2].next = &data[3];
	data[2].prev = &data[1]; */
	/* data[3].cmd = NULL;
	data[3].phrase = NULL;
	data[3].next = NULL;
	data[3].prev = &data[2]; */
	return (WEXITSTATUS(execute_cmds(data)));
} 

