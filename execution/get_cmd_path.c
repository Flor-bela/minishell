/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_cmd_path.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 23:02:00 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/20 16:16:33 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	**get_all_paths(char **envp)
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
	while (paths != NULL && paths[i] != NULL)
	{
		temp = ft_strjoin_modif(paths[i], "/");
		cmd_path = ft_strjoin_modif(temp, cmd_name);
		
		if (access(cmd_path, X_OK) == 0)
			return (cmd_path);
		i++;
	}

	return (NULL);
}

char	*get_cmd_path(t_cmdnode **data, char **envp, char *cmd_name, int **arr_fds, int len, char *line, t_intvec *codes)
{
	char	**paths;
	char	*cmd_path;
	char	*error;

	paths = get_all_paths(envp);
	cmd_path = find_path(paths, cmd_name);
	if (cmd_path == NULL)
	{
		if (find_chr(cmd_name, '/') == 1)
			error = ft_strjoin_modif("minishell: ", cmd_name);
		else
			error = cmd_name;
		error = ft_strjoin_modif(error, ": command not found\n");
		write(2, error, ft_strlen(error));
		close_fds(arr_fds, len);
		ft_free_malloc();
		free_envp_copy(envp);
		cmd_list_destroy(*data);
		free(line);
		intvec_destroy(codes);
		exit(127);
	}
	return (cmd_path);
}


