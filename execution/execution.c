/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 15:35:41 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/16 12:55:22 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	error_fork(char **envp)
{
	perror("fork");
	ft_free_malloc();
	change_exit_value("return_value=1", &envp);
	exit(EXIT_FAILURE);
}

static void	error_pipe(char **envp)
{
	perror("pipe");
	ft_free_malloc();
	change_exit_value("return_value=1", &envp);
	exit(EXIT_FAILURE);
}

 
static void	error_fd_file(char *name_infile, char **envp)
{
	perror(name_infile);
	ft_free_malloc();
	change_exit_value("return_value=1", &envp);
	exit(EXIT_FAILURE);
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
			change_exit_value("return_value=127", &envp);
			exit(127);
		}
	}
	else //-> si no tiene / entonces busca el path del comando y añade el nombre directamente a args[0];
	{
		cmd_path = get_cmd_path(envp, data_cmd, arr_fds, len);
		args[0] = data_cmd;
	}
	return (cmd_path);
}

static void	handle_infile_redirections(t_list_red **tmp_infiles, int **arr_fds, int len, char **envp)
{
	int			fd_file;

	while (*tmp_infiles != NULL)
	{
		fd_file = open((*tmp_infiles)->content, O_RDONLY);
		if (fd_file == -1)
		{
			close_fds(arr_fds, len);
			error_fd_file((*tmp_infiles)->content, envp);
		}
		dup2(fd_file, STDIN_FILENO);
		close (fd_file);
		*tmp_infiles = (*tmp_infiles)->next;
	}
}

static void	handle_outfile_redirections(t_list_red **tmp_outfiles, int **arr_fds, int len, char **envp)
{
	int	fd_file;
	
	while (*tmp_outfiles != NULL)
	{
		if ((*tmp_outfiles)->doble == 0)
			fd_file = open((*tmp_outfiles)->content, O_WRONLY | O_CREAT | O_TRUNC, 0666);
		if ((*tmp_outfiles)->doble == 1)
			fd_file = open((*tmp_outfiles)->content, O_WRONLY | O_CREAT | O_APPEND, 0666);
		if (fd_file == -1)
		{
			close_fds(arr_fds, len);
			error_fd_file((*tmp_outfiles)->content, envp);
		}
		dup2(fd_file, STDOUT_FILENO);
		close (fd_file);
		*tmp_outfiles= (*tmp_outfiles)->next;
	}
}


int	execute_cmds(t_cmdnode **data, char **envp)
{
	pid_t	*pids;
	char	**args;
	char	*cmd_path;
	int		len;
	int		i;
	int		last_exit_status;
	int		**arr_fds;
	t_cmdnode *data_tmp;

	len = 0;
	i = 0;
	last_exit_status = 0;
	data_tmp = *data;
	pids = NULL;
	while (data_tmp && data_tmp->cmd != NULL)
	{
		len++;
		data_tmp = data_tmp->next;
	}
	
	arr_fds = (int **)ft_malloc(sizeof(int *) * (len - 1)); // len has the amount of commands - 1 is the correct number for pipes
	while (i < len - 1)
	{
		arr_fds[i] = (int *)ft_malloc(sizeof(int) * 2); 
		if (pipe(arr_fds[i]) == -1)
			error_pipe(envp);
		i++;
	}
	
	i = 0;
	data_tmp = *data;
	if (len == 1 && (is_built_in(data_tmp->cmd) == 1))
	{
		t_list_red	*tmp_infiles;

		tmp_infiles = data_tmp->infiles;
		if (tmp_infiles != NULL)
			handle_infile_redirections(&tmp_infiles, arr_fds, len, envp);
		t_list_red	*tmp_outfiles;

		tmp_outfiles = data_tmp->outfiles;
		if (tmp_outfiles != NULL)
			handle_outfile_redirections(&tmp_outfiles, arr_fds, len, envp);

		//executar
		if (data_tmp && (ft_strncmp (data_tmp->cmd, "echo", 4) == 0 && data_tmp->cmd[4] == '\0'))
			builtin_echo(data_tmp->phrase);
		if (data_tmp && (ft_strncmp (data_tmp->cmd, "pwd", 3) == 0 && data_tmp->cmd[3] == '\0'))
			builtin_pwd();
		if (data_tmp && (ft_strncmp (data_tmp->cmd, "export", 6) == 0 && data_tmp->cmd[6] == '\0'))
			builtin_export(data_tmp->phrase, &envp);
		if (data_tmp && (ft_strncmp (data_tmp->cmd, "unset", 5) == 0 && data_tmp->cmd[5] == '\0'))
			builtin_unset(data_tmp->phrase, &envp);
		if (data_tmp && (ft_strncmp (data_tmp->cmd, "env", 3) == 0 && data_tmp->cmd[3] == '\0'))
			builtin_env(envp);
		if (data_tmp && (ft_strncmp (data_tmp->cmd, "exit", 4) == 0 && data_tmp->cmd[4] == '\0'))
			//should_exit = builtin_exit(data_tmp->phrase);
			return(builtin_exit(data_tmp->phrase)); // not sure about this....
		//close_fds(arr_fds, len); // I think there is no need
	}
	else
	{
		pids = ft_malloc(sizeof(pid_t) * len); // si es solo un y built-in no deberia crear pids....(quizás esto lo pongo dentro del loop???)

		while (i < len) // ir executando este loop para crear los procesos y ejecutar los comandos
		{
			pids[i] = fork();
			if (pids[i] == -1)
				error_fork(envp);
			else if (pids[i] == 0) // in child process
			{
				// 1st count how many arguments we have:
				args = get_args(data_tmp->phrase);
				
				// 2nd: obtain the name of the command to add it do args[0] to execute with execve and gets command path
				cmd_path = get_cmd_name_and_path(args, envp, data_tmp->cmd, arr_fds, len);
				
				// 3rd: check if we are in the first command, in the "middle" or in the last command (they will be different)
				if (i == 0) // first command and only one command
				{
					// infiles <
					t_list_red	*tmp_infiles;

					tmp_infiles = data_tmp->infiles;
					if (tmp_infiles != NULL)
						handle_infile_redirections(&tmp_infiles, arr_fds, len, envp);

					// outfiles >
					t_list_red	*tmp_outfiles;

					tmp_outfiles = data_tmp->outfiles;
					if (tmp_outfiles != NULL)
						handle_outfile_redirections(&tmp_outfiles, arr_fds, len, envp);
					if (len > 1 && data_tmp->outfiles == NULL) // si existe pipe escribe en el pipe, si no que escriba en el terminal
						dup2(arr_fds[0][1], STDOUT_FILENO);
				}
				else if (i != 0 && i == (len - 1)) // last command (i no puede ser 0 si no va a repetirse con el primero comando)
				{
					// infiles <
					t_list_red	*tmp_infiles;

					tmp_infiles = data_tmp->infiles;
					if (tmp_infiles != NULL)
						handle_infile_redirections(&tmp_infiles, arr_fds, len, envp);
					else // only reads from pipe if there are no infiles
						dup2(arr_fds[i - 1][0], STDIN_FILENO); // lee del pipe
					// escribe directamente en STDOUT -> o sea la terminal
					
					//outfiles >
					t_list_red	*tmp_outfiles;

					tmp_outfiles = data_tmp->outfiles;
					if (tmp_outfiles != NULL)
						handle_outfile_redirections(&tmp_outfiles, arr_fds, len, envp);
				}
				else // in the middle
				{
					t_list_red	*tmp_infiles;

					tmp_infiles = data_tmp->infiles;
					if (tmp_infiles != NULL)
						handle_infile_redirections(&tmp_infiles, arr_fds, len, envp);
					else
					{
						dup2(arr_fds[i - 1][0], STDIN_FILENO); // take the reading part of the previous pipe as standard input
						if (data_tmp->outfiles == NULL) // writes to pipe only if outfile doesn´t exist
							dup2(arr_fds[i][1], STDOUT_FILENO); // write in the writing part of the next pipe.
					}
					//outfiles >
					t_list_red	*tmp_outfiles;

					tmp_outfiles = data_tmp->outfiles;
					if (tmp_outfiles != NULL)
						handle_outfile_redirections(&tmp_outfiles, arr_fds, len, envp);
					else
						dup2(arr_fds[i][1], STDOUT_FILENO);
				}
				close_fds(arr_fds, len);
				
				// 4th: executes the command:
				if (execve(cmd_path, args, envp) < 0)
				{
					perror(cmd_path);
					ft_free_malloc();
					change_exit_value("return_value=126", &envp);
					exit(126);
				}
			}
			i++;
			data_tmp = data_tmp->next;
		}
		
		// Main:
		close_fds(arr_fds, len);
		i = 0;
		while (i < len)
		{
			waitpid(pids[i], &last_exit_status, 0);
			i++;
		}
		ft_free_malloc();
	}
	return (WEXITSTATUS(last_exit_status));
}

