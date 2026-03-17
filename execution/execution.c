/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 15:35:41 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/18 00:23:53 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	error_fork(char **envp)
{
	perror("minishell: fork"); // probar
	change_exit_value("1", &envp);
	exit(1);
}

static void	error_pipe(char **envp)
{
	perror("minishell: pipe"); // probar
	change_exit_value("1", &envp);
	exit(1);
}

 
static void	error_fd_file(char *name_infile, char **envp)
{
	char *error;
	
	error = ft_strjoin_modif("minishell: ", name_infile);
	perror(error);
	change_exit_value("1", &envp);
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
	while (tmp != NULL)
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
	
	cmd_path = NULL;
	if (find_chr(data_cmd, '/') == 1) // if it has / in the name of the command...
	{
		cmd_name = strchr_modif(data_cmd, '/');
		cmd_path = data_cmd;
		if (access(cmd_path, F_OK) != 0)
		{
			char *error;
			
			error = ft_strjoin_modif("minishell: ", data_cmd);
			perror(error);
			close_fds(arr_fds, len);
			change_exit_value("127", &envp);
			exit(127);
		}
		if (access(cmd_path, X_OK) != 0)
		{
			char *error;
			
			error = ft_strjoin_modif("minishell: ", data_cmd);
			perror(error);
			close_fds(arr_fds, len);
			change_exit_value("126", &envp);
			exit(126);
		}
		//if (access(cmd_path, X_OK) == 0)
		args[0] = cmd_name;

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
	int	fd_file;

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
	//while (data_tmp && data_tmp->cmd != NULL) // data && data_tmp->cmd anterior
	while (data_tmp != NULL)
	{
		len++;
		data_tmp = data_tmp->next;
	}
	//printf("len is %d\n", len);
	if (len > 0) // added this...in those cases where cmd is equal == 0
	{
		arr_fds = (int **)ft_malloc(sizeof(int *) * (len - 1)); // len has the amount of commands - 1 is the correct number for pipes
		while (i < len - 1)
		{
			arr_fds[i] = (int *)ft_malloc(sizeof(int) * 2); 
			if (pipe(arr_fds[i]) == -1)
				error_pipe(envp);
			i++;
		}
	}
	i = 0;
	data_tmp = *data;
	// Only redirections without commands and without pipes!
	if (len == 1 && data_tmp->cmd == NULL) // (if (len == 0)) cambie esto porque solo con infiles o outfiles, len ya va a ser == 1!!!
	{
		// infiles < 
		int	saved_stdin;
		int saved_stdout;

		saved_stdin = -1;
		saved_stdout = -1;
		if (data_tmp->infiles != NULL)
		{
			t_list_red	*tmp_infiles;
			int	fd_file;
			
			tmp_infiles = data_tmp->infiles;
			saved_stdin = dup(STDIN_FILENO);
			while (tmp_infiles != NULL)
			{
				fd_file = open(tmp_infiles->content, O_RDONLY);
				if (fd_file == -1)
				{
					char *error;
					
					error = ft_strjoin_modif("minishell: ", (char *)tmp_infiles->content);
					perror(error);
					return (1);
				}
				dup2(fd_file, STDIN_FILENO);
				close(fd_file);
				tmp_infiles = tmp_infiles->next;
			}
		}
		// outfiles >
		if (data_tmp->outfiles != NULL)
		{
			t_list_red	*tmp_outfiles;
			int	fd_file;
			
			tmp_outfiles = data_tmp->outfiles;
			saved_stdout = dup(STDOUT_FILENO);
			while (tmp_outfiles != NULL)
			{
				if (tmp_outfiles->doble == 0)
					fd_file = open(tmp_outfiles->content, O_WRONLY | O_CREAT | O_TRUNC, 0666);
				if (tmp_outfiles->doble == 1)
					fd_file = open(tmp_outfiles->content, O_WRONLY | O_CREAT | O_APPEND, 0666);
				if (fd_file == -1)
				{
					char *error;
					
					error = ft_strjoin_modif("minishell: ", (char *)tmp_outfiles->content);
					perror(error);
					return (1);
				}
				dup2(fd_file, STDOUT_FILENO);
				close (fd_file);
				tmp_outfiles = tmp_outfiles->next;
			}
		}
		///// 
		if (saved_stdin != -1)
		{
			dup2(saved_stdin, STDIN_FILENO);
			close(saved_stdin);
		}
		if (saved_stdout != -1)
		{
			dup2(saved_stdout, STDOUT_FILENO);
			close(saved_stdout);
		}
		return (0); // si se ejecuta bien???
	}
	
	// Only one command -> built-in!!!
	if (len == 1 && (is_built_in(data_tmp->cmd) == 1))
	{
		// infiles <
		t_list_red	*tmp_infiles;
		int 		saved_stdin;
		
		saved_stdin = -1;
		tmp_infiles = data_tmp->infiles;
		if (tmp_infiles != NULL)
		{
			int	fd_file;
			
			saved_stdin = dup(STDIN_FILENO);
			while (tmp_infiles != NULL)
			{
				fd_file = open(tmp_infiles->content, O_RDONLY);
				if (fd_file == -1)
				{
					close_fds(arr_fds, len);
					error_fd_file(tmp_infiles->content, envp);
				}
				dup2(fd_file, STDIN_FILENO);
				close(fd_file);
				tmp_infiles = tmp_infiles->next;
			}
		}
		// outfiles >
		t_list_red	*tmp_outfiles;
		int	saved_stdout;

		saved_stdout = -1;
		tmp_outfiles = data_tmp->outfiles;
		if (tmp_outfiles != NULL)
		{
			int	fd_file;
			
			saved_stdout = dup(STDOUT_FILENO);
			while (tmp_outfiles != NULL)
			{
				if (tmp_outfiles->doble == 0)
					fd_file = open(tmp_outfiles->content, O_WRONLY | O_CREAT | O_TRUNC, 0666);
				if (tmp_outfiles->doble == 1)
					fd_file = open(tmp_outfiles->content, O_WRONLY | O_CREAT | O_APPEND, 0666);
				if (fd_file == -1)
				{
					close_fds(arr_fds, len);
					error_fd_file(tmp_outfiles->content, envp);
				}
				dup2(fd_file, STDOUT_FILENO);
				close (fd_file);
				tmp_outfiles= tmp_outfiles->next;
			}
		}
		
		//ejecutar
		int	return_value;
		
		return_value = 0;
		if (data_tmp && (ft_strncmp (data_tmp->cmd, "echo", 4) == 0 && data_tmp->cmd[4] == '\0'))
			return_value = builtin_echo(data_tmp->phrase);
		else if (data_tmp && (ft_strncmp (data_tmp->cmd, "cd", 2) == 0 && data_tmp->cmd[2] == '\0'))
			return_value = builtin_cd(data_tmp->phrase->content);
		else if (data_tmp && (ft_strncmp (data_tmp->cmd, "pwd", 3) == 0 && data_tmp->cmd[3] == '\0'))
			return_value = builtin_pwd();
		else if (data_tmp && (ft_strncmp (data_tmp->cmd, "export", 6) == 0 && data_tmp->cmd[6] == '\0'))
			return_value = builtin_export(data_tmp->phrase, &envp);
		else if (data_tmp && (ft_strncmp (data_tmp->cmd, "unset", 5) == 0 && data_tmp->cmd[5] == '\0'))
			return_value = builtin_unset(data_tmp->phrase, &envp);
		else if (data_tmp && (ft_strncmp (data_tmp->cmd, "env", 3) == 0 && data_tmp->cmd[3] == '\0'))
			return_value = builtin_env(envp);
		else if (data_tmp && (ft_strncmp (data_tmp->cmd, "exit", 4) == 0 && data_tmp->cmd[4] == '\0'))
			return_value = builtin_exit(data_tmp->phrase);
		
		if (saved_stdin != -1)
		{
			dup2(saved_stdin, STDIN_FILENO);
			close(saved_stdin);
		}
		if (saved_stdout != -1)
		{
			dup2(saved_stdout, STDOUT_FILENO);
			close(saved_stdout);
		}
		return(return_value);
	}
	
	else //// Pipeline!!!! 
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
				if (data_tmp->cmd != NULL)
					args = get_args(data_tmp->phrase);
				else
					args = NULL;
				// 2nd: obtain the name of the command to add it do args[0] to execute with execve and gets command path
				if (data_tmp->cmd != NULL)
					cmd_path = get_cmd_name_and_path(args, envp, data_tmp->cmd, arr_fds, len);
				else
					cmd_path = NULL;
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
						/* if (data_tmp->outfiles == NULL) // writes to pipe only if outfile doesn´t exist
							dup2(arr_fds[i][1], STDOUT_FILENO); // write in the writing part of the next pipe. */ // creo que esto está duplicado...
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
				if (cmd_path != NULL && args != NULL)
				{
					if (execve(cmd_path, args, envp) < 0)
					{
						char *error;
						
						error = ft_strjoin_modif("minishell: ", cmd_path);
						perror(error);
						if (errno == EACCES)
						{	
							change_exit_value("126", &envp);
							exit(126);
						}
						else if (errno == ENOENT)
						{
							change_exit_value("127", &envp);
							exit(127);
						}
						else
						{
							change_exit_value("126", &envp);
							exit(126);}
					}
				}
				exit(0);
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
	}
	return (WEXITSTATUS(last_exit_status));
}
