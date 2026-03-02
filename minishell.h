/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 13:34:51 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/02 11:26:51 by oscar            ###   ########.fr       */
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

#include <string.h>
#include <ctype.h>

typedef struct s_cmdnode {
    char                *cmd;    //cd,					/usr/bin/cd
    char                *phrase;  //"hola mundo"			hola que
    struct s_cmdnode    *next;
	struct s_cmdnode   *prev;
} t_cmdnode;

typedef struct s_intvec {
    int   *data;				//"0,5,0,1,4,0,3,0"
    size_t len;
    size_t cap;
} t_intvec;



#endif

//cambiar programa para que meta las reedirecciones como parametro del nodo


