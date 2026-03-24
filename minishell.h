/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 13:34:51 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/24 17:45:30 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <errno.h>
# include <fcntl.h>
# include <signal.h>
# include <ctype.h>
# include <termios.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/wait.h>

typedef struct s_list
{
	void			*content;
	struct s_list	*prev;
	struct s_list	*next;
}					t_list;

typedef struct s_list_red
{
	void				*content;
	int					doble;
	int					is_heredoc;
	struct s_list_red	*prev;
	struct s_list_red	*next;
}					t_list_red;

typedef struct s_cmdnode
{
	char				*cmd;
	t_list				*phrase;
	t_list_red			*infiles;
	t_list_red			*outfiles;
	struct s_cmdnode	*next;
}						t_cmdnode;

typedef struct s_intvec
{
	int		*data;
	size_t	len;
	size_t	cap;
}			t_intvec;

typedef struct s_exit_ctx
{
	t_list		*args;
	int			in_child;
	char		***envp;
	t_cmdnode	**data;
	char		*line;
	t_intvec	*codes;
}	t_exit_ctx;

typedef struct s_parser
{
	size_t		i;
	size_t		j;
	size_t		op_len;
	int			op_code;
	int			op_match;
	int			last_pipe;
	t_cmdnode	*head;
	t_cmdnode	*tail;
	t_cmdnode	*current_cmd;
	t_list_red	*pending_in;
	t_list_red	*pending_out;
}	t_parser;

typedef struct s_heredoc_args
{
	t_parser	*parser;
	const char	*target;
	char		**env;
	t_cmdnode	**data;
	char		*line;
	t_intvec	*codes;
}	t_heredoc_args;

typedef struct s_parse_ctx
{
	const char	*line;
	t_parser	*p;
	t_intvec	*codes;
	char		**env;
	t_cmdnode	**data;
}	t_parse_ctx;

typedef struct s_exec_pack
{
	char		**envp;
	t_cmdnode	**data;
	char		*line;
	t_intvec	*codes;
	int			**arr_fds;
	int			len;
}	t_exec_pack;

typedef struct s_redir_params
{
	t_parse_ctx	*ctx;
	t_cmdnode	*cmd;
	int			op_code;
	const char	*target;
}	t_redir_params;

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

typedef struct s_add_redir_locals
{
	t_heredoc_args	args;
	t_heredoc_fork	ctx2;
	int				fd;
}	t_add_redir_locals;

extern volatile sig_atomic_t	g_sigint_received;

int			builtin_exit(t_exit_ctx *ctx);
int			builtin_cd(t_list *args, char **envp);
void		change_exit_value(char *args, char ***envp);

char		*read_heredoc_input(const char *delimiter,
				int expand_vars, char **shell_env);
char		*remove_quotes_str(const char *str);
int			parse_line(const char *line, t_intvec *out_codes,
				t_cmdnode **out_list, char **shell_env);
char		*get_env_value(char **envp, const char *key);
char		**dup_envp(char **envp);
void		free_envp_copy(char **envp);

t_list		*ft_lstnew(void *content);
void		ft_lstadd_back(t_list **lst, t_list *new);
void		*ft_malloc(size_t size);
void		ft_free_malloc(void);
size_t		env_len(char **envp);

char		**ft_split(char const *s, char c);
size_t		ft_strlen(const char *s);
size_t		ft_strlcpy(char *dst, const char *src, size_t size);
int			find_chr(const char *s, int c);
char		*strchr_modif(const char *s, int c);
char		*ft_strjoin_modif(char const *s1, char const *s2);
char		*ft_strjoin(char const *s1, char const *s2);
int			ft_strncmp(const char *s1, const char *s2, size_t n);
char		*ft_itoa(int n);
void		*ft_memset(void *s, int c, size_t n);
char		*ft_strdup(const char *s);
void		*ft_memcpy(void *dest, const void *src, size_t n);
void		*ft_realloc(void *ptr, size_t old_size, size_t new_size);
int			ft_strcmp(const char *s1, const char *s2);
char		*ft_strndup(const char *s, size_t n);
int			ft_isdigit(int c);
int			ft_atoi(const char *nptr);
char		*ft_strchr(const char *s, int c);

char		*get_cmd_path(t_exec_pack *p, char *cmd_name);
int			execute_cmds(t_cmdnode **data, char ***envp,
				char *line, t_intvec *codes);
void		close_fds(int **arr_fds, int len_cmds);

int			is_built_in(char *cmd_name);

void		handle_sigint(int sig);

void		handle_sigint_heredoc(int sig);
void		setup_signals_heredoc(void);

void		cmd_list_destroy(t_cmdnode *h);
void		intvec_destroy(t_intvec *v);
void		free_envp_copy(char **envp);

/* list utils */
void		intvec_push(t_intvec *v, int x);
void		intvec_free(t_intvec *v);
void		str_list_push_back(t_list **head, const char *s);
void		str_list_free(t_list *lst);
t_list_red	*red_list_push_back(t_list_red **head, const char *s, int doble);
void		red_list_free(t_list_red *lst);

/* cmd utils */
t_cmdnode	*node_new(const char *cmd);
void		cmd_list_append(t_cmdnode **head, t_cmdnode **tail, t_cmdnode *n);
void		cmd_list_free(t_cmdnode *h);
void		move_red_list_to_back(t_list_red **dst, t_list_red **src);

/* token */
void		skip_spaces(const char *s, size_t *i);
char		*dup_range(const char *s, size_t a, size_t b);
char		*read_token_grouped(const char *s, size_t *i, int *grouped_out);
void		update_token_quote(const char *s, size_t *i,
				char *quote, int *grouped_out);

/* expand */
char		*expand_env_vars(const char *token, char **env);
char		*append_char_dyn(char *base, char c);
char		*append_str_dyn(char *base, const char *add);

/* operators */
int			match_op(const char *s, size_t i, size_t *len, int *code);

/* redirections */
int			add_redir_target(t_redir_params *p);
int			store_pending_redir(t_parse_ctx *ctx, const char *target);

/* operator handler */
int			handle_operator(t_parse_ctx *ctx);

/* command */
int			handle_command(const char *line, t_parser *p,
				t_intvec *codes, char **env);

/* parser entry */
int			parse_line(const char *line, t_intvec *codes,
				t_cmdnode **out_list, char **env);

void		parser_init(t_parser *p, t_cmdnode **out_list);
int			parser_error(t_parser *p, t_intvec *codes);
void		finalize_pending(t_parser *p);
int			parse_loop(t_parse_ctx *ctx);
char		*get_clean_token(const char *line, size_t *i, char **env);

char		**dup_envp(char **envp);
char		*get_env_value(char **envp, const char *key);
void		free_envp_copy(char **envp);

int			is_valid_identifier(const char *s);
int			find_env_index(char **envp, const char *key);
int			replace_env_value(char **envp, int idx, const char *new_value);
int			append_env_value(char ***envp, const char *new_value);
int			remove_env_index(char ***envp, int idx);
void		print_export_env(char **envp);

void		change_exit_value(char *value, char ***envp);

/* ===================== STRING ===================== */

char		*remove_quotes_str(const char *str);
char		*join_and_free(char *s1, const char *s2);
void		print_without_outer_quotes(char *str);
int			is_n_flag(char *s);

/* ===================== EXPANSION ===================== */

char		*expand_heredoc_line(const char *line, char **env);

/* ===================== HEREDOC ===================== */

char		*read_heredoc_input(const char *delimiter,
				int expand_vars, char **shell_env);
char		*read_fd_all(int fd);

int	heredoc_with_fork(t_heredoc_fork *ctx, t_heredoc_args *args);

/* ===================== BUILTINS ===================== */

int			builtin_echo(t_list *args);
int			builtin_pwd(void);
int			builtin_export(t_list *args, char ***envp);
int			builtin_unset(t_list *args, char ***envp);
int			builtin_env(char **envp);
int			builtin_cd(t_list *args, char **envp);

/* ===================== BUILTIN EXIT UTILS ===================== */

int			is_numeric(char *str);

size_t		env_len(char **envp);

char		*append_char_local(char *base, char c);
char		*append_str_local(char *base, const char *add);
int			is_var_start_local(char c);
int			is_var_char_local(char c);
char		*get_var_value(const char *line, size_t *i, char **env);
int			init_heredoc_ctx(t_heredoc_ctx *ctx,
				const char *delimiter, int expand, char **env);
void		heredoc_child_process(t_heredoc_fork *ctx,
				t_heredoc_args *args, int fd[2]);
int			check_sigint(char *line, char *content);
int			process_heredoc_line(t_heredoc_ctx *ctx);
int			check_heredoc_signal(int status, char ***env, int fd);
void		exit_cleanup(t_exit_ctx *ctx);
int			count_args(t_list *args);
void		exit_numeric_error(t_exit_ctx *ctx, char *str);
int			exit_too_many_args(t_exit_ctx *ctx);
int			init_heredoc_ctx2(t_heredoc_fork *ctx, t_heredoc_args *args);

void		setup_signals_prompt(void);
void		setup_signals_heredoc(void);
int			process_input(char **line);
int			handle_parse(char *line, t_intvec *codes,
				t_cmdnode **list, char ***env);
void		cleanup_iteration(t_cmdnode *list,
				t_intvec *codes, char *line);
void		cmd_list_destroy(t_cmdnode *h);
void		intvec_destroy(t_intvec *v);
int			read_line_select(char **line, int has_prev);
char		*append_line(char *full, char *line, char *quote);
int			handle_pipe1(t_parser *p);
int			handle_redirection(t_parse_ctx *ctx);

int			handle_infile_redirections(t_list_red **tmp_infiles,
				t_exec_pack *p, t_heredoc_args *args, char ***envp);
void		handle_outfile_redirections(t_list_red **tmp_outfiles,
				t_exec_pack *p);
int			is_quoted(const char *s);
void		freedom(void **ptr);
int			ft_isalpha(int c);
int			ft_isalnum(int c);

#endif
