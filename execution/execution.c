/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 15:35:41 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/13 11:54:06 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	error_fork(void)
{
	perror("fork"); // fork is what is written in the manual for fork
	ft_free_malloc();
	exit(EXIT_FAILURE);
}

static void	error_pipe(void)
{
	perror("pipe"); // ??
	ft_free_malloc();
	exit(EXIT_FAILURE);
}

 
static void	error_fd_file(char *name_infile)
{
	perror(name_infile);
	ft_free_malloc();
	exit(1);
}


char	**get_args(t_list	*phrase)
{
	//1st count how many arguments we have (t_list->content)
	t_list	*tmp;
	int		args_count;
	char	**args;
	int		i;

	args_count = 0;
	i = 0;
	args = NULL;
	tmp = phrase;
	while (tmp != NULL) // count arguments
	{
		args_count++;
		tmp = tmp->next;
	}
	args = (char **)ft_malloc(sizeof(char *) * (args_count + 2)); // + 2 para añadir el array con nombre del comando (args[0]) y el ultimo array que es el del caracter nulo
	tmp = phrase;
	while (i < args_count)
	{
		args[i + 1] = tmp->content;
		tmp = tmp->next;
		i++;
	}
	args[args_count + 1] = NULL;
	return (args);
}


static char	*get_cmd_name_and_path(char **args, char **envp, char *data_cmd, int **arr_fds, int len) // adds args[0] has the name of the command and gets command path
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
			close_fds(arr_fds, len);
			ft_free_malloc();
			// guardar el codigo
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
	t_cmdnode *tmp;

	len = 0;
	i = 0;
	status = 0;
	tmp = data;
	while (tmp && tmp->cmd != NULL)
	{
		len++; // si tengo todos estos comandos, entonces tengo que crear el numero de cmds - 1 para crear todos los pipes
		tmp = tmp->next;
	}
	//printf("len is %d\n", len);
	arr_fds = (int **)ft_malloc(sizeof(int *) * (len - 1)); // len has the amount of commands - 1 is the correct number for pipes
	while (i < len - 1)
	{
		arr_fds[i] = (int *)ft_malloc(sizeof(int) * 2); 
		if (pipe(arr_fds[i]) == -1)
			error_pipe();
		i++;
	}

	pids = ft_malloc(sizeof(pid_t) * len); // si es solo un y built-in no deberia crear pids....(quizás esto lo pongo dentro del loop???)
	i = 0;
	
	while (i < len) // ir executando este loop para crear los procesos y ejecutar los comandos
	{
		// SOMEWHERE HERE -> IF LEN == 1 and IS_BUILT_IN == 1 -> then do not fork
		//if (len == 1 && (is_built_in(data[i].cmd) == 1))
		// executar aqui directamente sin forks!!!!
		
		
		//else -> seria lo demás -> hmmm pero no me gusta así, es raro
		pids[i] = fork();
		if (pids[i] == -1)
			error_fork();
		else if (pids[i] == 0) // in child process
		{
			//printf("Executing command %d\n", i);
			
			//1st split phrase into words to create an array of words to send to execve (we still miss args[0]):
			//args = split_phrase(data[i].phrase);
			
			// 1st count how many arguments we have:
			args = get_args(data[i].phrase);
			
			// 2nd: obtain the name of the command to add it do args[0] to execute with execve and gets command path
			cmd_path = get_cmd_name_and_path(args, envp, data[i].cmd, arr_fds, len);
			//printf(" cmd name is %s\n", args[0]);
			// 3rd: check if we are in the first command, in the "middle" or in the last command (they will be different)
			if (i == 0) // first command and only one command
			{
				// redirection <
				int		fd_file;
				t_list_red	*tmp_infiles;
				
				tmp_infiles = data[i].infiles;
				if (tmp_infiles != NULL)
				{
					while (tmp_infiles != NULL)
					{
						fd_file = open(tmp_infiles->content, O_RDONLY);
						if (fd_file == -1)
						{
							close_fds(arr_fds, len);
							error_fd_file(tmp_infiles->content);
						}
						dup2(fd_file, STDIN_FILENO);
						close (fd_file);
						tmp_infiles = tmp_infiles->next; // moverme hasta que no tenga más ningun infile
					}
				}
				// outfiles >
				t_list_red	*tmp_outfiles;

				tmp_outfiles = data[i].outfiles;
				if (tmp_outfiles != NULL)
				{
					while (tmp_outfiles != NULL)
					{
						if (tmp_outfiles->doble == 0)
							fd_file = open(tmp_outfiles->content, O_WRONLY | O_CREAT | O_TRUNC, 0666); //changed flags from pipex here
						if (tmp_outfiles->doble == 1)
							fd_file = open(tmp_outfiles->content, O_WRONLY | O_CREAT | O_APPEND, 0666); //changed flags from pipex here
						if (fd_file == -1)
						{
							close_fds(arr_fds, len);
							error_fd_file(tmp_outfiles->content);
						}
						dup2(fd_file, STDOUT_FILENO);
						close (fd_file);
						tmp_outfiles= tmp_outfiles->next; // moverme hasta que no tenga más ningun infile
					}
				}
				// if (len > 1 && data[i].outfiles == NULL) // solo escribe en pipe si no tiene outfiles
				if (len > 1 && data[i].outfiles == NULL) // si existe pipe escribe en el pipe, si no que escriba en el terminal
					dup2(arr_fds[0][1], STDOUT_FILENO);
			}
			else if (i != 0 && i == (len - 1)) // last command (i no puede ser 0 si no va a repetirse con el primero comando)
			{
				int		fd_file;
				t_list_red	*tmp_infiles;

				tmp_infiles = data[i].infiles;
				if (tmp_infiles != NULL) //redirections <
				{
					while (tmp_infiles != NULL)
					{
						fd_file = open(tmp_infiles->content, O_RDONLY);
						if (fd_file == -1)
						{
							close_fds(arr_fds, len);
							error_fd_file(tmp_infiles->content);
						}
						dup2(fd_file, STDIN_FILENO);
						close (fd_file);
						tmp_infiles = tmp_infiles->next; // moverme hasta que no tenga más ningun infile
					} 
				}
				else // only reads from pipe if there are no infiles
					dup2(arr_fds[i - 1][0], STDIN_FILENO); // lee del pipe
				// escribe directamente en STDOUT -> o sea la terminal
				
				//outfiles >
				t_list_red	*tmp_outfiles;

				tmp_outfiles = data[i].outfiles;
				if (tmp_outfiles != NULL)
				{
					while (tmp_outfiles != NULL)
					{
						if (tmp_outfiles->doble == 0)
							fd_file = open(tmp_outfiles->content, O_WRONLY | O_CREAT | O_TRUNC, 0666); //changed flags from pipex here
						if (tmp_outfiles->doble == 1)
							fd_file = open(tmp_outfiles->content, O_WRONLY | O_CREAT | O_APPEND, 0666); //changed flags from pipex here						
						if (fd_file == -1)
						{
							close_fds(arr_fds, len);
							error_fd_file(tmp_outfiles->content);
						}
						dup2(fd_file, STDOUT_FILENO);
						close (fd_file);
						tmp_outfiles= tmp_outfiles->next; // moverme hasta que no tenga más ningun infile
					}
				}
			}
			else // in the middle
			{
				int		fd_file;
				t_list_red	*tmp_infiles;
				
				tmp_infiles = data[i].infiles;
				if (tmp_infiles != NULL)
				{
					while (tmp_infiles != NULL)
					{
						fd_file = open(tmp_infiles->content, O_RDONLY);
						if (fd_file == -1)
						{
							close_fds(arr_fds, len);
							error_fd_file(tmp_infiles->content);
						}
						dup2(fd_file, STDIN_FILENO);
						close (fd_file);
						tmp_infiles = tmp_infiles->next; // moverme hasta que no tenga más ningun infile
					}
				}
				else
				{
					dup2(arr_fds[i - 1][0], STDIN_FILENO); // take the reading part of the previous pipe as standard input
					if (data[i].outfiles == NULL) // writes to pipe only if outfile doesn´t exist
						dup2(arr_fds[i][1], STDOUT_FILENO); // write in the writing part of the next pipe.
				}
				//outfiles
				t_list_red	*tmp_outfiles;

				tmp_outfiles = data[i].outfiles;
				if (tmp_outfiles != NULL)
				{
					while (tmp_outfiles != NULL)
					{
						if (tmp_outfiles->doble == 0)
							fd_file = open(tmp_outfiles->content, O_WRONLY | O_CREAT | O_TRUNC, 0666); //changed flags from pipex here
						if (tmp_outfiles->doble == 1)
							fd_file = open(tmp_outfiles->content, O_WRONLY | O_CREAT | O_APPEND, 0666); //changed flags from pipex here
						if (fd_file == -1)
						{
							close_fds(arr_fds, len);
							error_fd_file(tmp_outfiles->content);
						}
						dup2(fd_file, STDOUT_FILENO);
						close (fd_file);
						tmp_outfiles= tmp_outfiles->next; // moverme hasta que no tenga más ningun infile
					}
				}
				else
					dup2(arr_fds[i][1], STDOUT_FILENO); // write in the writing part of the next pipe.
			}
			close_fds(arr_fds, len); // cierra todos los fds
			
			// 4th: executes the command:
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

