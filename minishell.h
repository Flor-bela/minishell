/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <fda-roch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 13:34:51 by fda-roch          #+#    #+#             */
/*   Updated: 2026/02/04 14:59:07 by fda-roch         ###   ########.fr       */
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
typedef struct s_cmdnode
{
	char				*cmd;
	char				*phrase;
	struct s_cmdnode	*prev;
	struct s_cmdnode	*next;
}						t_cmdnode;

typedef struct s_intvec
{
	int		*data;
	size_t	len;
	size_t	cap;
}			t_intvec;

typedef struct s_list
{
	void			*content;
	struct s_list	*prev;
	struct s_list	*next;
}					t_list;

#endif