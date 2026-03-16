/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_cmd_path.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 23:02:00 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/16 17:49:14 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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
	char	*cmd_path;
	char	*temp;
	int		i;

	i = 0;
	cmd_path = NULL;
	temp = NULL;
	while (paths != NULL && paths[i] != NULL) // ir probando cual destes path tiene permiso
	{
		temp = ft_strjoin(paths[i], "/");
		cmd_path = ft_strjoin(temp, cmd_name);
		
		if (access(cmd_path, X_OK) == 0)
			return (cmd_path);
		i++;
	}

	return (NULL);
}

char	*get_cmd_path(char **envp, char *cmd_name, int **arr_fds, int len) // cmd_name = data[i].cmd
{
	char	**paths;
	char	*cmd_path;
	char	*error;
	
	paths = get_all_paths(envp);
	cmd_path = find_path(paths, cmd_name);
	if (cmd_path == NULL)
	{
		error = ft_strjoin(cmd_name, ": command not found\n");
		write(2, error, ft_strlen(error));
		close_fds(arr_fds, len);
		ft_free_malloc();
		change_exit_value("127", &envp);
		exit(127);
	}
	return (cmd_path);
}


