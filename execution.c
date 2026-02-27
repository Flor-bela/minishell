/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <fda-roch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 15:35:41 by fda-roch          #+#    #+#             */
/*   Updated: 2026/02/27 18:07:43 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	error_fork(void)
{
	perror("fork"); // fork is what is written in the manual for fork
	ft_free_malloc();
	exit(EXIT_FAILURE);
}

static void	error_pipe(void)
{
	perror("pipe");
	ft_free_malloc();
	exit(EXIT_FAILURE);
}

static	char **split_phrase(char *cmd_phrase)
{
	char	**split;
	char	**args;
	int		word_count;
	int		i;

	split = NULL;
	args = NULL;
	word_count = 0;
	i = 0;
	split = ft_split(cmd_phrase, ' ');
	while (split[word_count] != NULL)
		word_count++;

	args = (char **)ft_malloc(sizeof(char *) * (word_count + 2)); // + 2 para añadir el array con nombre del comando (args[0]) y el ultimo array que es el del caracter nulo
	while (i < word_count)
	{
		args[i + 1] = split[i];
		i++;
	}
	args[word_count + 1] = NULL;
	return (args);
}

static char	*check_cmd_name_path(char **args, char **envp, char *data_cmd) // adds args[0] has the name of the command and gets command path
{
	char	*cmd_name;
	char	*cmd_path;
	char	*error;

	if (find_chr(data_cmd, '/') == 1) // if it has / in the name of the command...
	{
		cmd_name = strchr_modif(data_cmd, '/');
		cmd_path = data_cmd;
		if (access(cmd_path, X_OK) == 0)
			args[0] = cmd_name;
		else // when there is no access to this path -> error
		{
			error = ft_strjoin(cmd_name, ": command not found\n");
			write(2, error, ft_strlen(error));
			ft_free_malloc();
			//close(fds[0]);
			//close(fds[1]);
			//close(fd_file);
			exit(127);
		}
	}
	else //-> si no tiene / entonces busca el path del comando y añade el nombre directamente a args[0];
	{
		cmd_path = get_cmd_path(envp, data_cmd);
		args[0] = data_cmd;
		// buscar el path ahora (pasar la copia de **envp de main para que la pueda usar aquí)
	}
	return (cmd_path);
}

int	execute_cmds(t_cmdnode *data, char **envp)
{
	pid_t	*pids;
	char	**args;
	char	*cmd_path;
	int		len;
	int		i;
	int		status;
	int		**arr_fds;
		
	len = 1;
	i = 0;
	status = 0;
	while (data[len].next != NULL)
		len++; // si tengo todos estos comandos, entonces tengo que crear el numero de cmds - 1 para crear todos los pipes
	
	arr_fds = (int **)ft_malloc(sizeof(int *) * (len - 1)); // len has the amount of commands - 1 is the correct number for pipes
	while (i < len - 1)
	{
		arr_fds[i] = (int *)ft_malloc(sizeof(int) * 2); 
		if (pipe(arr_fds[i]) == -1)
			error_pipe();
		printf("created pipe nº %d\n", i);
		printf("arr_fds [i][0] %d y arr_fds [i][1] %d\n", arr_fds[i][0], arr_fds[i][1]);
		i++;
	}

	pids = ft_malloc(sizeof(pid_t) * len);
	i = 0;
	while (i < len) // ir executando este loop para crear los procesos y ejecutar los comandos
	{
		pids[i] = fork();
		if (pids[i] == -1)
			error_fork();
		else if (pids[i] == 0) // in child process
		{
			printf("Executing command %d\n", i);
			
			//1st split phrase into words to create an array of words to send to execve (we still miss args[0]):
			args = split_phrase(data[i].phrase);
			
			// 2nd: obtain the name of the command to add it do args[0] to execute with execve and gets command path
			cmd_path = check_cmd_name_path(args, envp, data[i].cmd);
			
			// 3rd: executes the command:
			if (execve(cmd_path, args, envp) < 0)
			{
				perror(cmd_path);
				ft_free_malloc();
				exit(126);
			}
		}
		i++;
	}
	
	i = 0;
	while (i < len)
	{
		waitpid(pids[i], &status, 0);
		i++;
	}
	ft_free_malloc();
	return (status);
}

int	main(int argc, char **argv, char **envp)
{
	t_cmdnode	data[4];
	char		*cmd0 = "/usr/bin/echo";
	char		*cmd1 = "echo";
	char		*cmd2 = "ls";
	char		phrase0[] = "Hola Mundo";
	char		phrase1[] = "6";
	char		phrase2[] = "-a -l";
	
	
	(void)argc;
	(void)argv;
	
	data[0].cmd = cmd0;
	data[0].phrase = phrase0;
	data[0].next = &data[1];
	data[1].cmd = cmd1;
	data[1].phrase = phrase1;
	data[1].next = &data[2];
	data[2].cmd = NULL;
	data[2].phrase = NULL;
	data[2].next = NULL;
	data[2].cmd = cmd2;
	data[2].phrase = phrase2;
	data[2].next = &data[3];
	data[3].cmd = NULL;
	data[3].phrase = NULL;
	data[3].next = NULL;
	return (WEXITSTATUS(execute_cmds(data, envp)));
} 

