/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 13:34:51 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/13 10:17:34 by oscar            ###   ########.fr       */
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
}					t_cmdnode;

typedef struct s_intvec
{
	int		*data;
	size_t	len;
	size_t	cap;
}			t_intvec;

int		builtin_echo(char *phrase);
int		builtin_echo_remake(t_list *args);
int		builtin_pwd(void);
int		builtin_export(t_list *args, char ***envp);
int		builtin_unset(t_list *args, char ***envp);
int		builtin_env(char **envp);
int		builtin_exit(t_list *args);

char	*read_heredoc_input(const char *delimiter, int expand_vars, char **shell_env);
char	*remove_quotes_str(const char *str);
int		parse_line(const char *line, t_intvec *out_codes,
			t_cmdnode **out_list, char **shell_env);
char	*get_env_value(char **envp, const char *key);
char	**dup_envp(char **envp);
void	free_envp_copy(char **envp);

t_list	*ft_lstnew(void *content);
void	ft_lstadd_back(t_list **lst, t_list *new);
void	*ft_malloc(size_t size);
void	ft_free_malloc(void);

char	**ft_split(char const *s, char c);
size_t	ft_strlen(const char *s);
size_t	ft_strlcpy(char *dst, const char *src, size_t size);
int		find_chr(const char *s, int c);
char	*strchr_modif(const char *s, int c);
char	*ft_strjoin(char const *s1, char const *s2);
int		ft_strncmp(const char *s1, const char *s2, size_t n);

char	*get_cmd_path(char **envp, char *cmd_name, int **arr_fds, int len);
int		execute_cmds(t_cmdnode *data, char **envp);
void	close_fds(int **arr_fds, int len_cmds);

#endif