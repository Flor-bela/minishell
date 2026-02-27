/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <fda-roch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 13:34:51 by fda-roch          #+#    #+#             */
/*   Updated: 2026/02/27 17:32:07 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <errno.h>
# include <fcntl.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/wait.h>

/*
* Structs
*/
/* typedef struct s_cmdnode // tokens
{
	char				*cmd;
	char				*phrase;
	struct s_cmdnode	*prev;
	struct s_cmdnode	*next;
}						t_cmdnode; */

typedef struct s_list
{
	void			*content;
	struct s_list	*prev;
	struct s_list	*next;
}					t_list;

typedef struct s_cmdnode // tokens
{
	char				*cmd;
	char				*phrase;
	t_list				*infiles;
	t_list				*outfiles;
	//int					append; //??? Como sé cuando aparece y donde ??
	struct s_cmdnode	*next;
}						t_cmdnode;

/* typedef struct s_cmdnode // esta estructura sería mejor...
{
	int			pid; // opcional
	char		*cmd;
	char		**params; // ya separados en un array seria mejor, pero yo tambien lo puedo hacer en execucion
	t_list		*infiles;
	t_list		*outfiles;
	int			append;
	struct name	*next;
}				t_cmdnode; */

typedef struct s_intvec
{
	int		*data;
	size_t	len;
	size_t	cap;
}			t_intvec;

//void	count_cmds(t_intvec array);
//int		count_pipes(t_intvec array);

int		builtin_echo(char *phrase);

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

char	*get_cmd_path(char **envp, char *cmd_name);

int		execute_cmds(t_cmdnode *data, char **envp);

//int		main (int argc, char **argv, char **envp);

#endif