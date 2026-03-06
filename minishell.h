/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 13:34:51 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/06 11:58:45 by oscar            ###   ########.fr       */
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

typedef struct s_list
{
    void            *content;
    struct s_list   *prev;
    struct s_list   *next;
}   t_list;

typedef struct s_cmdnode {
   char     *cmd;
    t_list  *phrase;
    t_list  *infiles; // < (redirecciones)
    t_list  *outfiles; // > (redirecciones)
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
                            //camiar *phrase a lista para pasar          hola "que tal" estas
                            //hacer que en las phrase se añadan las comillas        phrase=""hola como estas""
//juntar las ramas
                            //hacer que al haber <<< salga error
                            //hacer que al haber || o mas salga error
                            //hacer que compruebe que acabe las comillas simples o dobles


//(1 t_cmdnode)


//hacer que se pueda leer los .txt




//distincion con variables locales $
