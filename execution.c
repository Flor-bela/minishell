/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <fda-roch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 15:35:41 by fda-roch          #+#    #+#             */
/*   Updated: 2026/02/24 16:50:05 by fda-roch         ###   ########.fr       */
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
int	execute_cmds(t_cmdnode *data, char **envp) // taking command as its absolute path directly
{
	int	i;
	i = 0;
	
	while (data[i].next != NULL)
	{
		printf("cmd %d %s\n", i, data[i].cmd);
		i++;
	}
	
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
			
				
			//i = 0;
			// here obtain the name of the command:
			char	*cmd_path;

			if (find_chr(data[i].cmd, '/') == 1) // if it has / in the name of the command...
			{
				char	*cmd_name;
				cmd_name = strchr_modif(data[i].cmd, '/');
				cmd_path = data[i].cmd;
				if (access(cmd_path, X_OK) == 0) // still need to see if we have access to it!!!
				{
					args[0] = cmd_name;
					printf("With / -> Command nº %d name is %s\n", i, args[0]);
				}
				else // when there is no access to this path
				{
					char	*error;

					error = ft_strjoin(cmd_name, ": command not found\n");
					write(2, error, ft_strlen(error));
					free(error);
					//close(fds[0]);
					//close(fds[1]);
					//close(fd_file);
					exit(127);
				}
			}
			else //-> si no tiene / entonces busca el path del comando y añade el nombre directamente a args[0];
			{
				cmd_path = get_cmd_path(envp, data[i].cmd);
				args[0] = data[i].cmd;
				printf("Without / -> Command nº %d name is %s\n", i, args[0]);
				// buscar el path ahora (pasar la copia de **envp de main para que la pueda usar aquí)
				
			}	
			
			int j;
			j = 0;
			while (j < word_count)
			{
				args[j + 1] = split[j];
				j++;
			}
			args[word_count + 1] = NULL;
			
			
			if (execve(cmd_path, args, NULL) < 0)
			{
				perror(cmd_path);
				//free(data.cmd); // liberar todo si no funciona!!! Muy importante hacerlo inmediatamente después
				//ft_error_args(args);
				exit(126); // 126??
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

int	main(int argc, char **argv, char **envp)
{
	t_cmdnode	data[4];
	char		*cmd0 = "/usr/bin/echo";
	char		*cmd1 = "echo";
	char		*cmd2 = "ls";
	char		phrase0[] = "Hola Mundo";
	char		phrase1[] = "World Wide Web";
	char		phrase2[] = "-a";
	
	
	(void)argc;
	(void)argv;
	
	data[0].cmd = cmd0;
	data[0].phrase = phrase0;
	data[0].prev = &data[3];
	data[0].next = &data[1];
	data[1].prev = &data[0];
	data[1].cmd = cmd1;
	data[1].phrase = phrase1;
	data[1].next = &data[2];
	data[2].cmd = NULL;
	data[2].phrase = NULL;
	data[2].next = NULL;
	data[2].prev = &data[1];
	data[2].cmd = cmd2;
	data[2].phrase = phrase2;
	data[2].next = &data[3];
	data[2].prev = &data[1];
	data[3].cmd = NULL;
	data[3].phrase = NULL;
	data[3].next = NULL;
	data[3].prev = &data[2];
	return (WEXITSTATUS(execute_cmds(data, envp)));
} 

