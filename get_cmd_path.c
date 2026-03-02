/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_cmd_path.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 23:02:00 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/02 11:54:33 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* static void	free_cmd_temp(char **cmd_temp)
{
	int	i;

	i = 0;
	while (cmd_temp != NULL && cmd_temp[i] != NULL)
	{
		free(cmd_temp[i]);
		i++;
	}
	free(cmd_temp);
} */

/* static void	free_paths(char **paths)
{
	int	i;

	i = 0;
	while (paths != NULL && paths[i] != NULL)
	{
		free(paths[i]);
		i++;
	}
	free(paths);
} */

static char	**get_all_paths(char **envp) // in the variable PATH= we have many different paths there, we need to split them all here to then later try them all and see if the command we are looking for is in one of them
{
	char	**paths;
	char	*full_path;
	int		i;

	i = 0;
	paths = NULL;
	full_path = NULL;
	while (envp && envp[i] != NULL)
	{
		if (ft_strncmp(envp[i], "PATH=", 5) == 0)
		{
			full_path = envp[i] + 5;
			break ;
		}
		i++;
	}
	paths = ft_split(full_path, ':');
	return (paths);
}

static char	*find_path(char **paths, char *cmd_name)
{
	//char	**cmd_temp;
	char	*cmd_path;
	char	*temp;
	int		i;

	i = 0;
	cmd_path = NULL;
	temp = NULL;
	while (paths != NULL && paths[i] != NULL) // ir probando cual destes path tiene aceso, o sea tiene el comando?
	{
		temp = ft_strjoin(paths[i], "/");
		cmd_path = ft_strjoin(temp, cmd_name);
		//free(temp);
		if (access(cmd_path, X_OK) == 0)
		{
			//free_cmd_temp(cmd_temp); // ver esto!!! no necesitaría hacer free aquí con ft_malloc creo...
			return (cmd_path);
		}
		i++;
		//free(cmd_path); // // tampoco necesitaria hacer free aqui creo!
	}
	//free_cmd_temp(cmd_temp);
	return (NULL);
}

//char	*ft_get_cmd_path(char **envp, char *argv, int *fds, int fd_file)
// ft_strjoin tiene malloc real!!! ver esto! 
char	*get_cmd_path(char **envp, char *cmd_name) // cmd_name = data[i].cmd
{
	char	**paths;
	char	*cmd_path;
	char	*error;
	//int		i;

	//i = 0;
	paths = get_all_paths(envp);
	cmd_path = find_path(paths, cmd_name);
	if (cmd_path == NULL)
	{
		//free_paths(paths); // ver esto!!! tengo ft_split con ft_malloc que es lo que crea este paths...
		error = ft_strjoin(cmd_name, ": command not found\n");
		write(2, error, ft_strlen(error));
		//free(error);
		//close(fds[0]);
		//close(fds[1]);
		//close(fd_file);
		ft_free_malloc();
		exit(127); // command not found
	}
	//free_paths(paths);
	return (cmd_path);
}
