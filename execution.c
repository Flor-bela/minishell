/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 15:35:41 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/06 19:29:24 by fda-roch         ###   ########.fr       */
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
		//printf("created pipe nº %d\n", i);
		//printf("arr_fds [i][0] %d y arr_fds [i][1] %d\n", arr_fds[i][0], arr_fds[i][1]);
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
			//printf("Executing command %d\n", i);
			
			//1st split phrase into words to create an array of words to send to execve (we still miss args[0]):
			args = split_phrase(data[i].phrase);
			
			// 2nd: obtain the name of the command to add it do args[0] to execute with execve and gets command path
			cmd_path = check_cmd_name_path(args, envp, data[i].cmd, arr_fds, len);
			//printf("args[0] %s\n", args[0]);
			//printf("cmd path %s\n", cmd_path);
			// 3rd: check if we are in the first command, in the "middle" or in the last command (they will be different)
			//printf(" cmd path is %s\n", cmd_path);
			//printf(" args is %s\n", args[0]);
			if (i == 0) // first command and only one command
			{
				// redirection <
				int		fd_file;
				t_list	*tmp_infiles;
				
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
				// outfiles
				t_list	*tmp_outfiles;

				tmp_outfiles = data[i].outfiles;
				if (tmp_outfiles != NULL)
				{
					while (tmp_outfiles != NULL)
					{
						fd_file = open(tmp_outfiles->content, O_WRONLY | O_CREAT | O_TRUNC, 0666); //changed flags from pipex here
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
				t_list	*tmp_infiles;

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
				else // only reads from pipe if there are no infiles
					dup2(arr_fds[i - 1][0], STDIN_FILENO); // lee del pipe
				// escribe directamente en STDOUT -> o sea la terminal
				
				//outfiles
				t_list	*tmp_outfiles;

				tmp_outfiles = data[i].outfiles;
				if (tmp_outfiles != NULL)
				{
					while (tmp_outfiles != NULL)
					{
						fd_file = open(tmp_outfiles->content, O_WRONLY | O_CREAT | O_TRUNC, 0666); //changed flags from pipex here
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
				t_list	*tmp_infiles;
				
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
				t_list	*tmp_outfiles;

				tmp_outfiles = data[i].outfiles;
				if (tmp_outfiles != NULL)
				{
					while (tmp_outfiles != NULL)
					{
						fd_file = open(tmp_outfiles->content, O_WRONLY | O_CREAT | O_TRUNC, 0666); //changed flags from pipex here
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

int	main(int argc, char **argv, char **envp)
{
	
	// 1st // cat < hello < world | grep .c | sort | wc -l
	/* t_cmdnode	data[4];
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
	data[0].infiles = &list[0];
	data[0].outfiles = NULL;
	data[0].next = &data[1];
	data[1].cmd = cmd1;
	data[1].phrase = phrase1;
	data[1].infiles = NULL;
	data[1].outfiles = NULL;
	data[1].next = &data[2];
	data[2].cmd = cmd2;
	data[2].phrase = phrase2;
	data[2].infiles = NULL;
	data[2].outfiles = NULL;
	data[2].next = &data[3];
	data[3].cmd = cmd3;
	data[3].phrase = phrase3;
	data[3].infiles = NULL;
	data[3].outfiles = NULL;
	data[3].next = NULL; */
 
 
	//////////////////////////////////////////////////////////////////////
	//2nd // cat < hello < world | grep world < otherfile (grep ignores pipe)

	/* t_cmdnode	data[3];
	char		*cmd0 = "cat";
	char		*cmd1 = "grep";
	char		phrase0[] = "";
	char		phrase1[] = "world";
	t_list		list[2];
	t_list		list2[1];
	
	(void)argc;
	(void)argv;
	list[0].content = (char *)"hello";
	list[0].next  = &list[1];
	list[1].content = (char *)"world";
	list[1].next = NULL;

	list2[0].content = (char *)"otherfile";
	list2[0].next = NULL;
	
	data[0].cmd = cmd0;
	data[0].phrase = phrase0;
	data[0].infiles = list;
	data[0].outfiles = NULL;
	data[0].next = &data[1];
	data[1].cmd = cmd1;
	data[1].phrase = phrase1;
	data[1].infiles = list2;
	data[1].outfiles = NULL;
	data[1].next = NULL; */



	/////////////////////////////////////
	// 3rd // cat hello | grep abc
	/* t_cmdnode	data[3];
	char		*cmd0 = "cat";
	char		*cmd1 = "grep";
	char		phrase0[] = "hello";
	char		phrase1[] = "abc";
	
	(void)argc;
	(void)argv;

	data[0].cmd = cmd0;
	data[0].phrase = phrase0;
	data[0].infiles = NULL;
	data[0].outfiles = NULL;
	data[0].next = &data[1];
	data[1].cmd = cmd1;
	data[1].phrase = phrase1;
	data[1].infiles = NULL;
	data[1].outfiles = NULL;
	data[1].next = NULL; */

	
	////////////////////////////////////////////
	// 4th // cat hello | grep world | wc -l
	/* t_cmdnode	data[4];
	char		*cmd0 = "cat";
	char		*cmd1 = "grep";
	char		*cmd2 = "wc";
	char		phrase0[] = "hello";
	char		phrase1[] = "world";
	char		phrase2[] = "-l";
	
	(void)argc;
	(void)argv;

	data[0].cmd = cmd0;
	data[0].phrase = phrase0;
	data[0].infiles = NULL;
	data[0].outfiles = NULL;
	data[0].next = &data[1];
	data[1].cmd = cmd1;
	data[1].phrase = phrase1;
	data[1].infiles = NULL;
	data[1].outfiles = NULL;
	data[1].next = &data[2];
	data[2].cmd = cmd2;
	data[2].phrase = phrase2;
	data[2].infiles = NULL;
	data[2].outfiles = NULL;
	data[2].next = NULL; */

	/////////////////////////////////////////////////

	//5th // yes | yes | yes | head -n 5 // yes prints yes in an infinite loop and head reads only the first 5 rows so it stops yes -> when head exits the pipe closed and yes receives SIGPIPE so it also stops
	/* t_cmdnode	data[4];
	char		*cmd0 = "yes";
	char		*cmd1 = "yes";
	char		*cmd2 = "yes";
	char		*cmd3 = "head";
	char		phrase0[] = "";
	char		phrase1[] = "";
	char		phrase2[] = "";
	char		phrase3[] = "-n 5";
	
	(void)argc;
	(void)argv;
	
	data[0].cmd = cmd0;
	data[0].phrase = phrase0;
	data[0].infiles = NULL;
	data[0].outfiles = NULL;
	data[0].next = &data[1];
	data[1].cmd = cmd1;
	data[1].phrase = phrase1;
	data[1].infiles = NULL;
	data[1].outfiles = NULL;
	data[1].next = &data[2];
	data[2].cmd = cmd2;
	data[2].phrase = phrase2;
	data[2].infiles = NULL;
	data[2].outfiles = NULL;
	data[2].next = &data[3];
	data[3].cmd = cmd3;
	data[3].phrase = phrase3;
	data[3].infiles = NULL;
	data[3].outfiles = NULL;
	data[3].next = NULL; */

	////////////////////////////////////////////////////////
	
	// 6th // cat < hello < world | grep pie | sort | uniq | grep pie | wc -l | cat
	/* 
	t_cmdnode	data[7];
	char		*cmd0 = "cat";
	char		*cmd1 = "grep";
	char		*cmd2 = "sort";
	char		*cmd3 = "uniq";
	char		*cmd4 = "grep";
	char		*cmd5 = "wc";
	char		*cmd6 = "cat";
	char		phrase0[] = "";
	char		phrase1[] = "apple";
	char		phrase2[] = "";
	char		phrase3[] = "";
	char		phrase4[] = "apple";
	char		phrase5[] = "-l";
	char		phrase6[] = "";
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
	data[1].cmd = cmd1;
	data[1].phrase = phrase1;
	data[1].infiles = NULL;
	data[1].outfiles = NULL;
	data[1].next = &data[2];
	data[2].cmd = cmd2;
	data[2].phrase = phrase2;
	data[2].infiles = NULL;
	data[2].outfiles = NULL;
	data[2].next = &data[3];
	data[3].cmd = cmd3;
	data[3].phrase = phrase3;
	data[3].infiles = NULL;
	data[3].outfiles = NULL;
	data[3].next = &data[4];
	data[4].cmd = cmd4;
	data[4].phrase = phrase4;
	data[4].infiles = NULL;
	data[4].outfiles = NULL;
	data[4].next = &data[5];
	data[5].cmd = cmd5;
	data[5].phrase = phrase5;
	data[5].infiles = NULL;
	data[5].outfiles = NULL;
	data[5].next = &data[6];
	data[6].cmd = cmd6;
	data[6].phrase = phrase6;
	data[6].infiles = NULL;
	data[6].outfiles = NULL;
	data[6].next = NULL;
 */

	// para los outfiles:
	//1st //cat < hello > aqui > abc | wc -l > outfile1 > outfile2 -> apple no existe en el pipe por eso da codigo 1 (echo $? = 1)
/* 	t_cmdnode	data[2];
	char		*cmd0 = "cat";
	char		*cmd1 = "wc";
	char		phrase0[] = "";
	char		phrase1[] = "-l";
	t_list		list[1];
	t_list		list2[2];
	t_list		list3[2];
	
	(void)argc;
	(void)argv;
	list[0].content = (char *)"hello";
	list[0].next  = NULL;
	

	list2[0].content = (char *)"aqui";
	list2[0].next = &list2[1];
	list2[1].content = (char *)"abc";
	list2[1].next = NULL;
	
	
	list3[0].content = (char *)"outfile1";
	list3[0].next = &list3[1];
	list3[1].content = (char *)"outfile2";
	list3[1].next = NULL;

	data[0].cmd = cmd0;
	data[0].phrase = phrase0;
	data[0].infiles = list;
	data[0].outfiles = list2;
	data[0].next = &data[1];
	data[1].cmd = cmd1;
	data[1].phrase = phrase1;
	data[1].infiles = NULL;
	data[1].outfiles = list3;
	data[1].next = NULL; */

	//////////////////////////////////////////////////////////
	//2nd echo "hello from cmd1" > out1.txt | echo "hello from cmd2" > out2.txt | echo "hello from cmd3" > out3.txt
	/* t_cmdnode	data[3];
	char		*cmd0 = "echo";
	char		*cmd1 = "echo";
	char		*cmd2 = "echo";
	char		phrase0[] = "hello from cmd1";
	char		phrase1[] = "hello from cmd2";
	char		phrase2[] = "hello from cmd3";
	t_list		list0[1];
	t_list		list1[1];
	t_list		list2[1];
	
	(void)argc;
	(void)argv;
	list0[0].content = (char *)"out1.txt";
	list0[0].next  = NULL;
	
	list1[0].content = (char *)"out2.txt";
	list1[0].next = NULL;
	
	list2[0].content = (char *)"out3.txt";
	list2[0].next = NULL;

	data[0].cmd = cmd0;
	data[0].phrase = phrase0;
	data[0].infiles = NULL;
	data[0].outfiles = list0;
	data[0].next = &data[1];
	data[1].cmd = cmd1;
	data[1].phrase = phrase1;
	data[1].infiles = NULL;
	data[1].outfiles = list1;
	data[1].next = &data[2];
	data[2].cmd = cmd2;
	data[2].phrase = phrase2;
	data[2].infiles = NULL;
	data[2].outfiles = list2;
	data[2].next = NULL; */


	//////////////////////////////////////////////
	// Mixing things

	//paste names.txt ages.txt | awk '$2 > 28' | sort > final.txt
	// PROBLEM here!!!
	/* t_cmdnode	data[3];
	char		*cmd0 = "paste";
	char		*cmd1 = "awk";
	char		*cmd2 = "sort";
	char		phrase0[] = "names.txt ages.txt";
	char		phrase1[] = "'$2 > 28'"; // problem with single quotes!!! If it is single, I need to treat it as a whole argument and do not separate it...
	char		phrase2[] = "";
	t_list		list0[1];

 */
	// 1st paste names.txt ages.txt | grep 30 | sort > final.txt
	/* t_cmdnode	data[3];
	char		*cmd0 = "paste";
	char		*cmd1 = "grep";
	char		*cmd2 = "sort";
	char		phrase0[] = "names.txt ages.txt";
	char		phrase1[] = "30"; 
	char		phrase2[] = "";
	t_list		list0[1];
	
	(void)argc;
	(void)argv;
	list0[0].content = (char *)"final.txt";
	list0[0].next  = NULL;
	
	data[0].cmd = cmd0;
	data[0].phrase = phrase0;
	data[0].infiles = NULL;
	data[0].outfiles = NULL;
	data[0].next = &data[1];
	data[1].cmd = cmd1;
	data[1].phrase = phrase1;
	data[1].infiles = NULL;
	data[1].outfiles = NULL;
	data[1].next = &data[2];
	data[2].cmd = cmd2;
	data[2].phrase = phrase2;
	data[2].infiles = NULL;
	data[2].outfiles = list0;
	data[2].next = NULL; */
	
	///////////////////////////////////////////
	//2nd: cat < txt1.txt < txt2.txt > txt3.txt | grep "abc" < txt4.txt > txt5.txt | sort < txt6.txt | uniq > txt7.txt
	t_cmdnode	data[4];
	char		*cmd0 = "cat";
	char		*cmd1 = "grep";
	char		*cmd2 = "sort";
	char		*cmd3 = "uniq";
	char		phrase0[] = "";
	char		phrase1[] = "abc";
	char		phrase2[] = "";
	char		phrase3[] = "";
	t_list		list0_in[2];
	t_list		list0_out[1];
	t_list		list1_in[1];
	t_list		list1_out[1];
	t_list		list2_in[1];
	t_list		list3_out[1];
	
	(void)argc;
	(void)argv;
	list0_in[0].content = (char *)"txt1.txt";
	list0_in[0].next = &list0_in[1];
	list0_in[1].content = (char *)"txt2.txt";
	list0_in[1].next = NULL;
	
	list0_out[0].content = (char *)"txt3.txt";
	list0_out[0].next = NULL;
	

	list1_in[0].content = (char *)"txt4.txt";
	list1_in[0].next = NULL;
	
	list1_out[0].content = (char *)"txt5.txt";
	list1_out[0].next = NULL;

	
	list2_in[0].content = (char *)"txt6.txt";
	list2_in[0].next = NULL;

	
	list3_out[0].content = (char *)"txt7.txt";
	list3_out[0].next = NULL;
	
	data[0].cmd = cmd0;
	data[0].phrase = phrase0;
	data[0].infiles = list0_in;
	data[0].outfiles = list0_out;
	data[0].next = &data[1];
	data[1].cmd = cmd1;
	data[1].phrase = phrase1;
	data[1].infiles = list1_in;
	data[1].outfiles = list1_out;
	data[1].next = &data[2];
	data[2].cmd = cmd2;
	data[2].phrase = phrase2;
	data[2].infiles = list2_in;
	data[2].outfiles = NULL;
	data[2].next = &data[3];
	data[3].cmd = cmd3;
	data[3].phrase = phrase3;
	data[3].infiles = NULL;
	data[3].outfiles = list3_out;
	data[3].next = NULL;
	
	return (WEXITSTATUS(execute_cmds(data, envp)));
} 

