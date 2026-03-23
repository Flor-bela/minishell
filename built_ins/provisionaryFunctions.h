/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   provisionaryFunctions.h                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 11:12:35 by oscar             #+#    #+#             */
/*   Updated: 2026/03/23                                 ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PROVISIONARYFUNCTIONS_H
# define PROVISIONARYFUNCTIONS_H

# include "../minishell.h"


/* ================= ENV ================= */

size_t	env_len(char **envp);
char	**dup_envp(char **envp);
char	*get_env_value(char **envp, const char *key);
void	free_envp_copy(char **envp);
void	change_exit_value(char *value, char ***envp);
int		remove_env_index(char ***envp, int idx);

/* ================= STR / QUOTES ================= */

char	*remove_quotes_str(const char *str);

/* ================= HEREDOC ================= */

char	*read_heredoc_input(const char *delimiter,
			int expand_vars, char **shell_env);
char	*read_fd_all(int fd);
char	*heredoc_with_fork(t_heredoc_fork *ctx,
			t_heredoc_args *args);

/* ================= BUILTINS ================= */

int		builtin_echo(t_list *args);
int		builtin_pwd(void);
int		builtin_export(t_list *args, char ***envp);
int		builtin_unset(t_list *args, char ***envp);
int		builtin_env(char **envp);
int		builtin_cd(t_list *args, char **envp);
int		builtin_exit(t_exit_ctx *ctx);
void	handle_fok_error(t_exec_pack *p, char *data_cmd);
void	handle_xok_error(t_exec_pack *p, char *data_cmd);

char	*ms_strdup(const char *s);
int	find_env_index(char **envp, const char *key);
int	replace_env_value(char **envp, int idx, const char *new_value);
int	append_env_value(char ***envp, const char *new_value);
size_t	key_len(const char *s);
char	**alloc_env_minus_one(size_t len);
char	*append_char_local(char *base, char c);
char	*append_str_local(char *base, const char *add);
void	trim_newline(char *line);
char	*expand_line_if_needed(char *line, int expand, char **env);
char	*append_line_content(char *content, char *line);
char	*get_next_line(int fd);
char	*append_buffer(char *result, char *buffer);
int	check_sigint(char *line, char *content);
char	*join_and_free(char *s1, const char *s2);
char	*expand_heredoc_line(const char *line, char **shell_env);
int	is_valid_identifier(const char *s);
void	print_export_env(char **envp);

#endif