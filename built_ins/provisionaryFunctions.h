/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   provisionaryFunctions.h                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkalgano <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 11:12:35 by oscar             #+#    #+#             */
/*   Updated: 2026/03/23 15:46:46 by dkalgano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PROVISIONARYFUNCTIONS_H
# define PROVISIONARYFUNCTIONS_H

# include "../minishell.h"

typedef struct s_heredoc_ctx
{
	char		*line;
	char		*content;
	const char	*delimiter;
	int			expand;
	char		**env;
	int			status;
}	t_heredoc_ctx;

typedef struct s_heredoc_fork
{
	char		*delimiter;
	int			expand;
	char		**env;
	t_cmdnode	**data;
	char		*line;
	t_intvec	*codes;
}	t_heredoc_fork;

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
int		builtin_cd(t_list *args, char **envp);
size_t	env_len(char **envp);

void	change_exit_value(char *args, char ***envp);
int		heredoc_with_fork(t_heredoc_fork *ctx, t_heredoc_args *args);

#endif
