/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <fda-roch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 15:35:41 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/04 17:57:33 by fda-roch         ###   ########.fr       */
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

 
static void	error_fd_file(char *name_infile)
{
	perror(name_infile);
	ft_free_malloc();
	exit(1);
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

static char	*check_cmd_name_path(char **args, char **envp, char *data_cmd, int **arr_fds, int len) // adds args[0] has the name of the command and gets command path
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
			close_fds(arr_fds, len); // tengo que cerrarlos todos aquí también!!!
			ft_free_malloc();
			//close(fds[0]);
			//close(fds[1]);
			//close(fd_file);
			exit(127);
		}
	}
	else //-> si no tiene / entonces busca el path del comando y añade el nombre directamente a args[0];
	{
		cmd_path = get_cmd_path(envp, data_cmd, arr_fds, len);
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
	printf("len %d\n", len);
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
			cmd_path = check_cmd_name_path(args, envp, data[i].cmd, arr_fds, len);
			printf("args[0] %s\n", args[0]);
			printf("cmd path %s\n", cmd_path);
			// 3rd: check if we are in the first command, in the "middle" or in the last command (they will be different)
			
			if (i == 0) // first command and only one command
			{
				// redirection <
				int		fd_file;

				fd_file = 0; // mejor iniciarlo
				if (data[i].infiles != NULL)
				{
					while (data[i].infiles->content != NULL)
					{
						printf("infile is %s\n", (char *)data[i].infiles->content);
						fd_file = open(data[i].infiles->content, O_RDONLY);
						if (fd_file == -1)
						{
							close_fds(arr_fds, len);
							error_fd_file(data[i].infiles->content);
						}
						dup2(fd_file, STDIN_FILENO);
						close (fd_file);
						data[i].infiles = data[i].infiles->next; // moverme hasta que no tenga más ningun infile
					}
				}
				if (arr_fds[i] != NULL) // ??? si existe pipe escribe en el pipe, si no que escriba en el terminal....????
				{
					printf("aqui??\n");
					dup2(arr_fds[i][1], STDOUT_FILENO); // std_outfile is now the write end of pipe
				}
			}
			else if (i != 0 && i == (len - 1)) // last command (i no puede ser 0 si no va a repetirse con el primero comando)
			{
				int		fd_file;
				
				fd_file = 0;
				if (data[i].infiles != NULL)
				{
					while (data[i].infiles->next != NULL)
					{
						fd_file = open(data[i].infiles->content, O_RDONLY);
						if (fd_file == -1)
						{
							close_fds(arr_fds, len);
							error_fd_file(data[i].infiles->content);
						}
						dup2(fd_file, STDIN_FILENO);
						close (fd_file);
						data[i].infiles = data[i].infiles->next; // moverme hasta que no tenga más ningun infile
					} 
				}
				dup2(arr_fds[i - 1][0], STDIN_FILENO); // lee del pipe
				// escribe directamente en STDOUT -> o sea la terminal
			}
			else // in the middle
			{
				int		fd_file;
				
				fd_file = 0;
				if (data[i].infiles != NULL)
				{
					while (data[i].infiles->next != NULL)
					{
						fd_file = open(data[i].infiles->content, O_RDONLY);
						if (fd_file == -1)
						{
							close_fds(arr_fds, len);
							error_fd_file(data[i].infiles->content);
						}
						dup2(fd_file, STDIN_FILENO);
						close (fd_file);
						data[i].infiles = data[i].infiles->next; // moverme hasta que no tenga más ningun infile
					}
				}
				dup2(arr_fds[i - 1][0], STDIN_FILENO); // take the reading part of the previous pipe as standard input
				dup2(arr_fds[i][1], STDOUT_FILENO); // write in the writing part of the next pipe.
			}
			close_fds(arr_fds, len); // cierra todos los fds
			
			// 4th: executes the command:
			printf("hereeeeeeeeeeeeeeeeeeeeee\n"); // PK NO ENTRA AQUI???
			printf("error?\n");
			if (execve(cmd_path, args, envp) < 0)
			{
				perror(cmd_path);
				ft_free_malloc();
				exit(126);
			}
		}
		i++;
	}
	close_fds(arr_fds, len);
	
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
	// crear variable infiles (t_list)
	/* t_cmdnode	data[5];
	char		*cmd0 = "cat";
	char		*cmd1 = "grep";
	char		*cmd2 = "sort";
	char		*cmd3 = "wc";
	char		phrase0[] = "";
	char		phrase1[] = ".c";
	char		phrase2[] = "";
	char		phrase3[] = "-l";
	t_list		list[2];
	
	
	
	(void)argc;
	(void)argv;
	list[0].content = (char *)"hello";
	list[0].next  = &list[1];
	list[1].content = (char *)"world";
	list[1].next = NULL;
	
	data[0].cmd = cmd0;
	data[0].phrase = phrase0;
	data[0].infiles = list;
	data[0].next = &data[1];
	data[1].cmd = cmd1;
	data[1].phrase = phrase1;
	data[1].next = &data[2];
	data[2].cmd = cmd2;
	data[2].phrase = phrase2;
	data[2].next = &data[3];
	data[3].cmd = cmd3;
	data[3].phrase = phrase3;
	data[3].next = &data[4];
	data[4].cmd = NULL;
	data[4].phrase = NULL;
	data[4].next = NULL;
 */


	t_cmdnode	data[2];
	char		*cmd0 = "/usr/bin/cat";
	char		phrase0[] = " ";
	t_list		list[2];
	
	(void)argc;
	(void)argv;
	list[0].content = (char *)"hello";
	list[0].next  = &list[1];
	list[1].content = (char *)"world";
	list[1].next = NULL;
	
	data[0].cmd = cmd0;
	data[0].phrase = phrase0;
	data[0].infiles = list;
	data[0].outfiles = NULL;
	data[0].next = &data[1];
	data[1].cmd = NULL;
	data[1].phrase = NULL;
	data[1].infiles = NULL;
	data[1].outfiles = NULL;
	data[1].next = NULL;

	
	return (WEXITSTATUS(execute_cmds(data, envp)));
} 

