## File Structure
```
copiaMinishell/
└── parse
    ├── parser_cmd_utils.c
    ├── parser_command.c
    ├── parser_core.c
    ├── parser_expand_utils.c
    ├── parser_expand.c
    ├── parser_list_redir.c
    ├── parser_list_utils.c
    ├── parser_operator_handler.c
    ├── parser_operator.c
    ├── parser_redir.c
    ├── parser_token_utils.c
    ├── parser_token.c
    └── parser.h
```

### `parse/parser.h`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 10:28:12 by oscar             #+#    #+#             */
/*   Updated: 2026/03/16 12:43:44 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
# define PARSER_H

# include "../minishell.h"

typedef struct s_parser
{
	size_t		i;
	size_t		j;
	size_t		op_len;
	int			op_code;
	int			op_match;
	t_cmdnode	*head;
	t_cmdnode	*tail;
	t_cmdnode	*current_cmd;
	t_list_red	*pending_in;
	t_list_red	*pending_out;
}	t_parser;

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
```

### `parse/parser_cmd_utils.c`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_cmd_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 10:12:54 by oscar             #+#    #+#             */
/*   Updated: 2026/03/17 12:24:33 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

void	move_red_list_to_back(t_list_red **dst, t_list_red **src)
{
	t_list_red	*tail;

	if (!src || !*src)
		return ;
	if (!*dst)
	{
		*dst = *src;
		*src = NULL;
		return ;
	}
	tail = *dst;
	while (tail->next)
		tail = tail->next;
	tail->next = *src;
	(*src)->prev = tail;
	*src = NULL;
}

t_cmdnode	*node_new(const char *cmd)
{
	t_cmdnode	*n;

	n = malloc(sizeof(*n));
	if (!n)
	{
		perror("malloc");
		exit(1);
	}
	n->cmd = NULL;
	if (cmd)
		n->cmd = ft_strdup(cmd);
	if (cmd && !n->cmd)
	{
		perror("ft_strdup");
		exit(1);
	}
	n->phrase = NULL;
	n->infiles = NULL;
	n->outfiles = NULL;
	n->next = NULL;
	return (n);
}

void	cmd_list_append(t_cmdnode **head, t_cmdnode **tail, t_cmdnode *n)
{
	if (!*head)
	{
		*head = n;
		*tail = n;
	}
	else
	{
		(*tail)->next = n;
		*tail = n;
	}
}

void	cmd_list_free(t_cmdnode *h)
{
	t_cmdnode	*nx;

	while (h)
	{
		nx = h->next;
		free(h->cmd);
		str_list_free(h->phrase);
		red_list_free(h->infiles);
		red_list_free(h->outfiles);
		free(h);
		h = nx;
	}
}

```

### `parse/parser_command.c`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_command.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 10:13:03 by oscar             #+#    #+#             */
/*   Updated: 2026/03/17 17:01:21 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

static int	add_command_token(const char *line, t_parser *p, char **env)
{
	int		grouped;
	char	*tok;
	char	*expanded;
	char	*clean;

	grouped = 0;
	tok = read_token_grouped(line, &p->i, &grouped);
	if (!tok)
		return (1);

	expanded = expand_env_vars(tok, env);
	free(tok);
	if (!expanded)
		return (1);

	clean = remove_quotes_str(expanded);
	free(expanded);
	if (!clean)
		return (1);

	// 🔥 CASO CLAVE: token vacío
	if (clean[0] == '\0')
	{
		free(clean);
		return (0); // ignorar completamente
	}

	// 🔥 asignar cmd si no existe
	if (!p->current_cmd)
	{
		p->current_cmd = node_new(clean);
		cmd_list_append(&p->head, &p->tail, p->current_cmd);
	}
	else if (!p->current_cmd->cmd)
	{
		p->current_cmd->cmd = ft_strdup(clean);
	}
	else
	{
		str_list_push_back(&p->current_cmd->phrase, clean);
	}

	free(clean);
	return (0);
}

static int	handle_tail_operator(const char *line, t_parser *p,
	t_intvec *codes, char **env)
{
	if (p->op_code == 5)
	{
		intvec_push(codes, p->op_code);
		p->i = p->j + p->op_len;
		p->current_cmd = NULL;
		return (0);
	}
	p->i = p->j;
	return (handle_operator(line, p, codes, env));
}

static int	fill_command_tail(const char *line, t_parser *p,
	t_intvec *codes, char **env)
{
	while (line[p->i])
	{
		p->j = p->i;
		skip_spaces(line, &p->j);
		if (!line[p->j])
		{
			p->i = p->j;
			return (0);
		}
		p->op_match = match_op(line, p->j, &p->op_len, &p->op_code);
		if (p->op_match == -1)
			return (-1);
		if (p->op_match == 1)
			return (handle_tail_operator(line, p, codes, env));
		p->i = p->j;
		if (add_command_token(line, p, env))
			return (0);
	}
	return (0);
}

int	handle_command(const char *line, t_parser *p,
				t_intvec *codes, char **env)
{
	if (!p->current_cmd)
	{
		p->current_cmd = node_new(NULL);
		cmd_list_append(&p->head, &p->tail, p->current_cmd);
	}

	if (add_command_token(line, p, env))
		return (1);

	// 🔥 SI DESPUÉS DEL TOKEN SIGUE SIN CMD → es vacío
	if (!p->current_cmd->cmd)
	{
		p->current_cmd->cmd = ft_strdup("");
	}

	return (0);
}

```

### `parse/parser_core.c`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_core.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 10:13:08 by oscar             #+#    #+#             */
/*   Updated: 2026/03/17 17:12:34 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

static void	parser_init(t_parser *p, t_cmdnode **out_list)
{
	p->i = 0;
	p->j = 0;
	p->head = NULL;
	p->tail = NULL;
	p->current_cmd = NULL;
	p->pending_in = NULL;
	p->pending_out = NULL;
	p->op_len = 0;
	p->op_code = -1;
	p->op_match = 0;
	*out_list = NULL;
}

static int	parser_error(t_parser *p, t_intvec *codes)
{
	cmd_list_free(p->head);
	red_list_free(p->pending_in);
	red_list_free(p->pending_out);
	intvec_free(codes);
	return (-1);
}

static void	finalize_pending(t_parser *p)
{
	if (p->pending_in || p->pending_out)
	{
		p->current_cmd = node_new(NULL);
		move_red_list_to_back(&p->current_cmd->infiles, &p->pending_in);
		move_red_list_to_back(&p->current_cmd->outfiles, &p->pending_out);
		cmd_list_append(&p->head, &p->tail, p->current_cmd);
	}
	p->current_cmd = p->head;
}

static int	parse_loop(const char *line, t_parser *p,
	t_intvec *codes, char **env)
{
	while (line[p->i])
	{
		skip_spaces(line, &p->i);
		if (!line[p->i])
			break ;
		p->op_match = match_op(line, p->i, &p->op_len, &p->op_code);
		if (p->op_match == -1)
			return (-1);
		if (p->op_match == 1)
		{
			if (handle_operator(line, p, codes, env) == -1)
				return (-1);
		}
		else
		{
			if (handle_command(line, p, codes, env) == -1)
				return (-1);
		}
	}
	finalize_pending(p);
	return (0);
}

int	parse_line(const char *line, t_intvec *codes,
			t_cmdnode **out_list, char **env)
{
	t_parser	p;
	int			last_was_pipe;

	parser_init(&p, out_list);
	last_was_pipe = 1;

	while (line[p.i])
	{
		skip_spaces(line, &p.i);
		if (!line[p.i])
			break ;

		// 🔥 DETECTAR OPERADOR
		p.op_match = match_op(line, p.i, &p.op_len, &p.op_code);
		if (p.op_match == -1)
			return (parser_error(&p, codes));

		if (p.op_match == 1)
		{
			// 🔥 CONTROL DE PIPE
			if (p.op_code == 5)
			{
				if (last_was_pipe)
				{
					write(2, "syntax error near unexpected token `|'\n", 40);
					return (parser_error(&p, codes));
				}
				last_was_pipe = 1;
			}
			else
				last_was_pipe = 0;

			if (handle_operator(line, &p, codes, env) == -1)
				return (parser_error(&p, codes));
		}
		else
		{
			last_was_pipe = 0;

			if (handle_command(line, &p, codes, env) == -1)
				return (parser_error(&p, codes));
		}
	}

	// 🔥 ERROR SI TERMINA EN PIPE
	if (last_was_pipe)
	{
		write(2, "syntax error near unexpected token `|'\n", 40);
		return (parser_error(&p, codes));
	}

	finalize_pending(&p);
	*out_list = p.head;
	return (0);
}
```

### `parse/parser_expand.c`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_expand.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 10:13:20 by oscar             #+#    #+#             */
/*   Updated: 2026/03/17 12:25:20 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

static int	is_var_start(char c)
{
	return (isalpha((unsigned char)c) || c == '_');
}

static int	is_var_char(char c)
{
	return (isalnum((unsigned char)c) || c == '_');
}

static char	*expand_variable(const char *token, size_t *i,
	char *res, char **env)
{
	size_t	start;
	char	*var_name;
	char	*value;

	if (!token[*i])
		return (append_char_dyn(res, '$'));
	if (token[*i] == '?')
	{
		value = get_env_value(env, "return_value");
		if (value)
			res = append_str_dyn(res, value);
		(*i)++;
		return (res);
	}
	if (!is_var_start(token[*i]))
		return (append_char_dyn(res, '$'));
	start = *i;
	while (token[*i] && is_var_char(token[*i]))
		(*i)++;
	var_name = dup_range(token, start, *i);
	value = get_env_value(env, var_name);
	free(var_name);
	if (value)
		res = append_str_dyn(res, value);
	return (res);
}

static char	update_quote(char quote, char c)
{
	if (c == '\'' && quote != '"')
	{
		if (quote == '\'')
			return (0);
		return ('\'');
	}
	if (c == '"' && quote != '\'')
	{
		if (quote == '"')
			return (0);
		return ('"');
	}
	return (quote);
}

char	*expand_env_vars(const char *token, char **env)
{
	size_t	i;
	char	*res;
	char	quote;

	i = 0;
	res = ft_strdup("");
	if (!res)
	{
		perror("ft_strdup");
		exit(1);
	}
	quote = 0;
	while (token[i])
	{
		quote = update_quote(quote, token[i]);
		if (token[i] == '$' && quote != '\'')
		{
			i++;
			res = expand_variable(token, &i, res, env);
			continue ;
		}
		res = append_char_dyn(res, token[i]);
		i++;
	}
	return (res);
}

```

### `parse/parser_expand_utils.c`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_expand_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 10:13:15 by oscar             #+#    #+#             */
/*   Updated: 2026/03/17 12:30:39 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

static char	*append_char_new(char c)
{
	char	*res;

	res = malloc(2);
	if (!res)
	{
		perror("malloc");
		exit(1);
	}
	res[0] = c;
	res[1] = 0;
	return (res);
}

char	*append_char_dyn(char *base, char c)
{
	size_t	len;
	char	*res;

	if (!base)
		return (append_char_new(c));
	len = ft_strlen(base);
	res = malloc(len + 2);
	if (!res)
	{
		perror("malloc");
		free(base);
		exit(1);
	}
	ft_memcpy(res, base, len);
	res[len] = c;
	res[len + 1] = 0;
	free(base);
	return (res);
}

static char	*append_str_new(const char *add)
{
	char	*res;

	res = ft_strdup(add);
	if (!res)
	{
		perror("ft_strdup");
		exit(1);
	}
	return (res);
}

char	*append_str_dyn(char *base, const char *add)
{
	size_t	len1;
	size_t	len2;
	char	*res;

	if (!add)
		add = "";
	if (!base)
		return (append_str_new(add));
	len1 = ft_strlen(base);
	len2 = ft_strlen(add);
	res = malloc(len1 + len2 + 1);
	if (!res)
	{
		perror("malloc");
		free(base);
		exit(1);
	}
	ft_memcpy(res, base, len1);
	ft_memcpy(res + len1, add, len2);
	res[len1 + len2] = 0;
	free(base);
	return (res);
}

```

### `parse/parser_list_redir.c`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_list_redir.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 10:13:25 by oscar             #+#    #+#             */
/*   Updated: 2026/03/17 12:25:40 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

void	str_list_free(t_list *lst)
{
	t_list	*next;

	while (lst)
	{
		next = lst->next;
		free(lst->content);
		free(lst);
		lst = next;
	}
}

static t_list_red	*red_node_new(const char *s, int doble)
{
	t_list_red	*node;

	node = malloc(sizeof(t_list_red));
	if (!node)
	{
		perror("malloc");
		exit(1);
	}
	if (s)
		node->content = ft_strdup(s);
	else
		node->content = NULL;
	if (s && !node->content)
	{
		perror("ft_strdup");
		exit(1);
	}
	node->doble = doble;
	node->prev = NULL;
	node->next = NULL;
	return (node);
}

static void	red_list_append(t_list_red **head, t_list_red **tail,
	const char *s, int doble)
{
	t_list_red	*node;

	node = red_node_new(s, doble);
	if (!*head)
	{
		*head = node;
		*tail = node;
	}
	else
	{
		node->prev = *tail;
		(*tail)->next = node;
		*tail = node;
	}
}

void	red_list_push_back(t_list_red **head, const char *s, int doble)
{
	t_list_red	*tail;

	tail = *head;
	if (tail)
	{
		while (tail->next)
			tail = tail->next;
	}
	red_list_append(head, &tail, s, doble);
}

void	red_list_free(t_list_red *lst)
{
	t_list_red	*next;

	while (lst)
	{
		next = lst->next;
		free(lst->content);
		free(lst);
		lst = next;
	}
}

```

### `parse/parser_list_utils.c`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_list_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 10:13:30 by oscar             #+#    #+#             */
/*   Updated: 2026/03/17 12:44:20 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

void	intvec_push(t_intvec *v, int x)
{
	size_t	newcap;
	int		*nd;

	if (v->len == v->cap)
	{
		if (v->cap == 0)
			newcap = 8;
		else
			newcap = v->cap * 2;
		nd = (int *)ft_realloc(v->data,
				v->cap * sizeof(int),
				newcap * sizeof(int));
		if (!nd)
		{
			perror("ft_realloc");
			exit(1);
		}
		v->data = nd;
		v->cap = newcap;
	}
	v->data[v->len++] = x;
}

void	intvec_free(t_intvec *v)
{
	free(v->data);
	v->data = NULL;
	v->len = 0;
	v->cap = 0;
}

static t_list	*str_node_new(const char *s)
{
	t_list	*node;

	node = malloc(sizeof(t_list));
	if (!node)
	{
		perror("malloc");
		exit(1);
	}
	if (s)
		node->content = ft_strdup(s);
	else
		node->content = NULL;
	if (s && !node->content)
	{
		perror("ft_strdup");
		exit(1);
	}
	node->prev = NULL;
	node->next = NULL;
	return (node);
}

static void	str_list_append(t_list **head, t_list **tail, const char *s)
{
	t_list	*node;

	node = str_node_new(s);
	if (!*head)
	{
		*head = node;
		*tail = node;
	}
	else
	{
		node->prev = *tail;
		(*tail)->next = node;
		*tail = node;
	}
}

void	str_list_push_back(t_list **head, const char *s)
{
	t_list	*tail;

	tail = *head;
	if (tail)
	{
		while (tail->next)
			tail = tail->next;
	}
	str_list_append(head, &tail, s);
}

```

### `parse/parser_operator.c`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_operator.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 10:13:42 by oscar             #+#    #+#             */
/*   Updated: 2026/03/17 13:26:13 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

static int	handle_less(const char *s, size_t i, size_t *len, int *code)
{
	if (s[i + 1] == '<')
	{
		if (s[i + 2] == '<')
		{
			write(2,
				"Error: no se permiten mas de 2 redirecciones '<' seguidas.\n",
				ft_strlen("Error: no se permiten mas de 2 redirecciones '<' seguidas.\n"));
			return (-1);
		}
		*len = 2;
		*code = 3;
		return (1);
	}
	*len = 1;
	*code = 1;
	return (1);
}

static int	handle_greater(const char *s, size_t i, size_t *len, int *code)
{
	if (s[i + 1] == '>')
	{
		if (s[i + 2] == '>')
		{
			write(2,
				"Error: no se permiten mas de 2 redirecciones '>' seguidas.\n",
				ft_strlen("Error: no se permiten mas de 2 redirecciones '>' seguidas.\n"));
			return (-1);
		}
		*len = 2;
		*code = 4;
		return (1);
	}
	*len = 1;
	*code = 2;
	return (1);
}

static int	handle_pipe(const char *s, size_t i, size_t *len, int *code)
{
	if (s[i + 1] == '|')
	{
		write(2, "Error: no se permiten 2 o mas pipes seguidos.\n", 47);
		return (-1);
	}
	*len = 1;
	*code = 5;
	return (1);
}

int	match_op(const char *s, size_t i, size_t *len, int *code)
{
	if (s[i] == '<')
		return (handle_less(s, i, len, code));
	if (s[i] == '>')
		return (handle_greater(s, i, len, code));
	if (s[i] == '|')
		return (handle_pipe(s, i, len, code));
	return (0);
}

```

### `parse/parser_operator_handler.c`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_operator_handler.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 10:13:35 by oscar             #+#    #+#             */
/*   Updated: 2026/03/17 17:16:36 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

static char	*get_redir_target(const char *line, size_t *i, char **env)
{
	char	*target;
	char	*expanded;
	int		grouped;

	grouped = 0;
	target = read_token_grouped(line, i, &grouped);
	if (!target)
	{
		printf("minishell: syntax error\n");
		change_exit_value("2", &env);
		return (NULL);
	}
	expanded = expand_env_vars(target, env);
	free(target);
	return (expanded);
}

static int	store_operator_target(t_parser *p, char *target, char **env)
{
	if (p->current_cmd)
		return (add_redir_target(p->current_cmd, p->op_code, target, env));
	return (store_pending_redir(p, p->op_code, target, env));
}

int	handle_operator(const char *line, t_parser *p,
	t_intvec *codes, char **env)
{
	char	*target;

	intvec_push(codes, p->op_code);
	p->i += p->op_len;

	// 🔥 CASO PIPE
	if (p->op_code == 5)
	{
		// mover redirecciones pendientes al nodo actual
		if (p->pending_in || p->pending_out)
		{
			if (!p->current_cmd)
			{
				p->current_cmd = node_new(NULL);
				cmd_list_append(&p->head, &p->tail, p->current_cmd);
			}
			move_red_list_to_back(&p->current_cmd->infiles, &p->pending_in);
			move_red_list_to_back(&p->current_cmd->outfiles, &p->pending_out);
		}

		// 🔥 CREAR NUEVO NODO SIEMPRE
		p->current_cmd = node_new(NULL);
		cmd_list_append(&p->head, &p->tail, p->current_cmd);

		return (0);
	}

	// 🔥 REDIRECCIONES
	target = get_redir_target(line, &p->i, env);
	if (!target)
		return (-1);

	if (p->current_cmd)
	{
		if (add_redir_target(p->current_cmd, p->op_code, target, env))
		{
			free(target);
			return (-1);
		}
	}
	else
	{
		if (store_pending_redir(p, p->op_code, target, env))
		{
			free(target);
			return (-1);
		}
	}

	free(target);
	return (0);
}
```

### `parse/parser_redir.c`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redir.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 10:13:49 by oscar             #+#    #+#             */
/*   Updated: 2026/03/17 12:53:08 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

static int	handle_heredoc(t_cmdnode *cmd,
	const char *target, char **env)
{
	char	*heredoc_content;
	char	*real_delim;
	int		expand_vars;

	expand_vars = 1;
	if (strchr(target, '\'') || strchr(target, '"'))
		expand_vars = 0;
	real_delim = remove_quotes_str(target);
	if (!real_delim)
		return (-1);
	heredoc_content = read_heredoc_input(real_delim, expand_vars, env);
	free(real_delim);
	if (!heredoc_content)
		return (-1);
	red_list_push_back(&cmd->infiles, heredoc_content, 1);
	free(heredoc_content);
	return (0);
}

static int	handle_pending_heredoc(t_parser *p,
	const char *target, char **env)
{
	char	*heredoc_content;
	char	*real_delim;
	int		expand_vars;

	expand_vars = 1;
	if (strchr(target, '\'') || strchr(target, '"'))
		expand_vars = 0;
	real_delim = remove_quotes_str(target);
	if (!real_delim)
		return (-1);
	heredoc_content = read_heredoc_input(real_delim, expand_vars, env);
	free(real_delim);
	if (!heredoc_content)
		return (-1);
	red_list_push_back(&p->pending_in, heredoc_content, 1);
	free(heredoc_content);
	return (0);
}

int	add_redir_target(t_cmdnode *cmd, int op_code,
	const char *target, char **env)
{
	if (op_code == 1)
		red_list_push_back(&cmd->infiles, target, 0);
	else if (op_code == 3)
		return (handle_heredoc(cmd, target, env));
	else if (op_code == 2)
		red_list_push_back(&cmd->outfiles, target, 0);
	else if (op_code == 4)
		red_list_push_back(&cmd->outfiles, target, 1);
	return (0);
}

int	store_pending_redir(t_parser *p, int op_code,
	const char *target, char **env)
{
	if (op_code == 1)
		red_list_push_back(&p->pending_in, target, 0);
	else if (op_code == 3)
		return (handle_pending_heredoc(p, target, env));
	else if (op_code == 2)
		red_list_push_back(&p->pending_out, target, 0);
	else if (op_code == 4)
		red_list_push_back(&p->pending_out, target, 1);
	return (0);
}

```

### `parse/parser_token.c`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_token.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 10:13:59 by oscar             #+#    #+#             */
/*   Updated: 2026/03/17 12:55:51 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

static int	handle_no_quote(const char *s, size_t *i,
	char *quote, int *grouped_out)
{
	size_t	op_len;
	int		op_code;

	if (match_op(s, *i, &op_len, &op_code) != 0
		|| isspace((unsigned char)s[*i]))
		return (1);
	if (s[*i] == '\'' || s[*i] == '"')
	{
		*quote = s[*i];
		if (grouped_out)
			*grouped_out = 1;
	}
	(*i)++;
	return (0);
}

static void	handle_in_quote(const char *s, size_t *i, char *quote)
{
	if (s[*i] == *quote)
		*quote = 0;
	(*i)++;
}

static int	read_token_loop(const char *s, size_t *i,
	char *quote, int *grouped_out)
{
	while (s[*i])
	{
		if (*quote == 0)
		{
			if (handle_no_quote(s, i, quote, grouped_out))
				break ;
		}
		else
			handle_in_quote(s, i, quote);
	}
	return (0);
}

char	*read_token_grouped(const char *s, size_t *i, int *grouped_out)
{
	size_t	start;
	size_t	op_len;
	int		op_code;
	int		op_match;
	char	quote;

	if (grouped_out)
		*grouped_out = 0;
	skip_spaces(s, i);
	if (!s[*i])
		return (NULL);
	op_match = match_op(s, *i, &op_len, &op_code);
	if (op_match != 0)
		return (NULL);
	start = *i;
	quote = 0;
	read_token_loop(s, i, &quote, grouped_out);
	return (dup_range(s, start, *i));
}

```

### `parse/parser_token_utils.c`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_token_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 10:13:54 by oscar             #+#    #+#             */
/*   Updated: 2026/03/17 12:30:39 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

void	skip_spaces(const char *s, size_t *i)
{
	while (s[*i] && isspace((unsigned char)s[*i]))
		(*i)++;
}

char	*dup_range(const char *s, size_t a, size_t b)
{
	size_t	n;
	char	*out;

	if (b < a)
		b = a;
	n = b - a;
	out = malloc(n + 1);
	if (!out)
	{
		perror("malloc");
		exit(1);
	}
	ft_memcpy(out, s + a, n);
	out[n] = 0;
	return (out);
}

void	update_token_quote(const char *s, size_t *i,
	char *quote, int *grouped_out)
{
	if (s[*i] == *quote)
		*quote = 0;
	else
	{
		*quote = s[*i];
		if (grouped_out)
			*grouped_out = 1;
	}
	(*i)++;
}

```

