/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 10:28:12 by oscar             #+#    #+#             */
/*   Updated: 2026/03/16 10:32:10 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
# define PARSER_H

# include "../minishell.h"

/* list utils */
void		intvec_push(t_intvec *v, int x);
void		intvec_free(t_intvec *v);
void		str_list_push_back(t_list **head, const char *s);
void		str_list_free(t_list *lst);
void		red_list_push_back(t_list_red **head, const char *s, int doble);
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
int			add_redir_target(t_cmdnode *cmd, int op_code,
				const char *target, char **env);
int			store_pending_redir(t_parser *p, int op_code,
				const char *target, char **env);

/* operator handler */
int			handle_operator(const char *line, t_parser *p,
				t_intvec *codes, char **env);

/* command */
int			handle_command(const char *line, t_parser *p,
				t_intvec *codes, char **env);

/* parser entry */
int			parse_line(const char *line, t_intvec *codes,
				t_cmdnode **out_list, char **env);

#endif