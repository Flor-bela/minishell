/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   provisionaryFunctions.h                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 11:12:35 by oscar             #+#    #+#             */
/*   Updated: 2026/03/18 11:41:56 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PROVISIONARYFUNCTIONS_H
# define PROVISIONARYFUNCTIONS_H

# include "../minishell.h"

char	**dup_envp(char **envp);
char	*get_env_value(char **envp, const char *key);
void	free_envp_copy(char **envp);

char	*remove_quotes_str(const char *str);
char	*read_heredoc_input(const char *delimiter,
			int expand_vars, char **shell_env);

int		builtin_echo(t_list *args);
int		builtin_pwd(void);
int		builtin_export(t_list *args, char ***envp);
int		builtin_unset(t_list *args, char ***envp);
int		builtin_env(char **envp);
int		builtin_exit(t_list *args);
//int		builtin_cd(char *path); // ongoing

void	change_exit_value(char *args, char ***envp);

#endif