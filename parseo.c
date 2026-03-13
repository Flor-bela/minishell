#include "minishell.h"

static void	intvec_push(t_intvec *v, int x)
{
	size_t	newcap;
	int		*nd;

	if (v->len == v->cap)
	{
		if (v->cap == 0)
			newcap = 8;
		else
			newcap = v->cap * 2;
		nd = (int *)realloc(v->data, newcap * sizeof(int));
		if (!nd)
		{
			perror("realloc");
			exit(1);
		}
		v->data = nd;
		v->cap = newcap;
	}
	v->data[v->len++] = x;
}

static void	intvec_free(t_intvec *v)
{
	free(v->data);
	v->data = NULL;
	v->len = 0;
	v->cap = 0;
}

static t_list	*str_node_new(const char *s)
{
	t_list	*node;

	node = (t_list *)malloc(sizeof(t_list));
	if (!node)
	{
		perror("malloc");
		exit(1);
	}
	if (s)
		node->content = strdup(s);
	else
		node->content = NULL;
	if (s && !node->content)
	{
		perror("strdup");
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

static void	str_list_push_back(t_list **head, const char *s)
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

static void	str_list_free(t_list *lst)
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

	node = (t_list_red *)malloc(sizeof(t_list_red));
	if (!node)
	{
		perror("malloc");
		exit(1);
	}
	if (s)
		node->content = strdup(s);
	else
		node->content = NULL;
	if (s && !node->content)
	{
		perror("strdup");
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

static void	red_list_push_back(t_list_red **head, const char *s, int doble)
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

static void	red_list_free(t_list_red *lst)
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

static void	move_red_list_to_back(t_list_red **dst, t_list_red **src)
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

static t_cmdnode	*node_new(const char *cmd)
{
	t_cmdnode	*n;

	n = (t_cmdnode *)malloc(sizeof(*n));
	if (!n)
	{
		perror("malloc");
		exit(1);
	}
	n->cmd = NULL;
	if (cmd)
		n->cmd = strdup(cmd);
	if (cmd && !n->cmd)
	{
		perror("strdup");
		exit(1);
	}
	n->phrase = NULL;
	n->infiles = NULL;
	n->outfiles = NULL;
	n->next = NULL;
	return (n);
}

static void	cmd_list_append(t_cmdnode **head, t_cmdnode **tail, t_cmdnode *n)
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

static void	cmd_list_free(t_cmdnode *h)
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

static void	skip_spaces(const char *s, size_t *i)
{
	while (s[*i] && isspace((unsigned char)s[*i]))
		(*i)++;
}

static char	*dup_range(const char *s, size_t a, size_t b_excl)
{
	size_t	n;
	char	*out;

	if (b_excl < a)
		b_excl = a;
	n = b_excl - a;
	out = (char *)malloc(n + 1);
	if (!out)
	{
		perror("malloc");
		exit(1);
	}
	memcpy(out, s + a, n);
	out[n] = '\0';
	return (out);
}

static int	is_var_start(char c)
{
	return (isalpha((unsigned char)c) || c == '_');
}

static int	is_var_char(char c)
{
	return (isalnum((unsigned char)c) || c == '_');
}

static char	*append_char_dyn(char *base, char c)
{
	size_t	len;
	char	*res;

	if (!base)
	{
		res = (char *)malloc(2);
		if (!res)
		{
			perror("malloc");
			exit(1);
		}
		res[0] = c;
		res[1] = '\0';
		return (res);
	}
	len = strlen(base);
	res = (char *)malloc(len + 2);
	if (!res)
	{
		perror("malloc");
		free(base);
		exit(1);
	}
	memcpy(res, base, len);
	res[len] = c;
	res[len + 1] = '\0';
	free(base);
	return (res);
}

static char	*append_str_dyn(char *base, const char *add)
{
	size_t	len1;
	size_t	len2;
	char	*res;

	if (!add)
		add = "";
	if (!base)
	{
		res = strdup(add);
		if (!res)
		{
			perror("strdup");
			exit(1);
		}
		return (res);
	}
	len1 = strlen(base);
	len2 = strlen(add);
	res = (char *)malloc(len1 + len2 + 1);
	if (!res)
	{
		perror("malloc");
		free(base);
		exit(1);
	}
	memcpy(res, base, len1);
	memcpy(res + len1, add, len2);
	res[len1 + len2] = '\0';
	free(base);
	return (res);
}

static char	*expand_env_vars(const char *token, char **shell_env)
{
	size_t	i;
	size_t	start;
	char	*res;
	char	quote;
	char	*var_name;
	char	*value;

	i = 0;
	res = strdup("");
	if (!res)
	{
		perror("strdup");
		exit(1);
	}
	quote = 0;
	while (token[i])
	{
		if (token[i] == '\'' && quote != '"')
		{
			if (quote == '\'')
				quote = 0;
			else
				quote = '\'';
			res = append_char_dyn(res, token[i]);
			i++;
		}
		else if (token[i] == '"' && quote != '\'')
		{
			if (quote == '"')
				quote = 0;
			else
				quote = '"';
			res = append_char_dyn(res, token[i]);
			i++;
		}
		else if (token[i] == '$' && quote != '\'')
		{
			i++;
			if (!token[i])
			{
				res = append_char_dyn(res, '$');
				break ;
			}
			if (token[i] == '?')
			{
				value = get_env_value(shell_env, "cp_envp");
				if (value)
					res = append_str_dyn(res, value);
				i++;
				continue ;
			}
			if (!is_var_start(token[i]))
			{
				res = append_char_dyn(res, '$');
				continue ;
			}
			start = i;
			while (token[i] && is_var_char(token[i]))
				i++;
			var_name = dup_range(token, start, i);
			value = get_env_value(shell_env, var_name);
			free(var_name);
			if (value)
				res = append_str_dyn(res, value);
		}
		else
		{
			res = append_char_dyn(res, token[i]);
			i++;
		}
	}
	return (res);
}

static int	match_op(const char *s, size_t i, size_t *op_len, int *op_code)
{
	if (s[i] == '<')
	{
		if (s[i + 1] == '<')
		{
			if (s[i + 2] == '<')
			{
				fprintf(stderr,
					"Error: no se permiten mas de 2 redirecciones '<' seguidas.\n");
				return (-1);
			}
			*op_len = 2;
			*op_code = 3;
			return (1);
		}
		*op_len = 1;
		*op_code = 1;
		return (1);
	}
	if (s[i] == '>')
	{
		if (s[i + 1] == '>')
		{
			if (s[i + 2] == '>')
			{
				fprintf(stderr,
					"Error: no se permiten mas de 2 redirecciones '>' seguidas.\n");
				return (-1);
			}
			*op_len = 2;
			*op_code = 4;
			return (1);
		}
		*op_len = 1;
		*op_code = 2;
		return (1);
	}
	if (s[i] == '|')
	{
		if (s[i + 1] == '|')
		{
			fprintf(stderr,
				"Error: no se permiten 2 o mas pipes seguidos.\n");
			return (-1);
		}
		*op_len = 1;
		*op_code = 5;
		return (1);
	}
	return (0);
}

static char	*read_token_grouped(const char *s, size_t *i, int *grouped_out)
{
	size_t	start;
	size_t	op_len;
	int		op_code;
	char	quote;
	int		op_match;

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
	while (s[*i])
	{
		if (quote == 0)
		{
			op_match = match_op(s, *i, &op_len, &op_code);
			if (op_match != 0)
				break ;
			if (isspace((unsigned char)s[*i]))
				break ;
			if (s[*i] == '\'' || s[*i] == '"')
			{
				quote = s[*i];
				if (grouped_out)
					*grouped_out = 1;
			}
			(*i)++;
		}
		else
		{
			if (s[*i] == quote)
				quote = 0;
			(*i)++;
		}
	}
	if (quote != 0)
	{
		fprintf(stderr, "Error: comillas sin cerrar.\n");
		return (NULL);
	}
	return (dup_range(s, start, *i));
}

static int	add_redir_target(t_cmdnode *cmd, int op_code, const char *target, char **shell_env)
{
	char	*heredoc_content;
	char	*real_delim;
	int		expand_vars;

	
	if (op_code == 1)
		red_list_push_back(&cmd->infiles, target, 0);
	else if (op_code == 3)
	{
		expand_vars = 1;
		if (strchr(target, '\'') || strchr(target, '"'))
			expand_vars = 0;
		real_delim = remove_quotes_str(target);
		if (!real_delim)
			return (-1);
		heredoc_content = read_heredoc_input(real_delim, expand_vars, shell_env);
		free(real_delim);
		if (!heredoc_content)
			return (-1);
		red_list_push_back(&cmd->infiles, heredoc_content, 1);
		free(heredoc_content);
	}
	else if (op_code == 2)
		red_list_push_back(&cmd->outfiles, target, 0);
	else if (op_code == 4)
		red_list_push_back(&cmd->outfiles, target, 1);
	return (0);
}

int	parse_line(const char *line, t_intvec *out_codes,
	t_cmdnode **out_list, char **shell_env)
{
	size_t		i;
	size_t		op_len;
	int			op_code;
	int			op_match;
	t_cmdnode	*head;
	t_cmdnode	*tail;
	t_cmdnode	*current_cmd;
	t_list_red	*pending_in;
	t_list_red	*pending_out;
	int			grouped;
	char		*cmd;
	char		*target;
	char		*tok;
	char		*expanded;

	i = 0;
	op_len = 0;
	op_code = -1;
	head = NULL;
	tail = NULL;
	current_cmd = NULL;
	pending_in = NULL;
	pending_out = NULL;
	*out_list = NULL;
	while (line[i])
	{
		skip_spaces(line, &i);
		if (!line[i])
			break ;
		op_match = match_op(line, i, &op_len, &op_code);
		if (op_match == -1)
		{
			cmd_list_free(head);
			red_list_free(pending_in);
			red_list_free(pending_out);
			intvec_free(out_codes);
			return (-1);
		}
		if (op_match == 1)
		{
			intvec_push(out_codes, op_code);
			i += op_len;
			if (op_code == 5)
			{
				current_cmd = NULL;
				continue ;
			}
			grouped = 0;
			target = read_token_grouped(line, &i, &grouped);
			if (!target)
			{
				fprintf(stderr, "Error: redirección sin destino.\n");
				cmd_list_free(head);
				red_list_free(pending_in);
				red_list_free(pending_out);
				intvec_free(out_codes);
				return (-1);
			}
			expanded = expand_env_vars(target, shell_env);
			free(target);
			target = expanded;
			if (current_cmd)
			{
				if (add_redir_target(current_cmd, op_code, target, shell_env) == -1)
				{
					free(target);
					cmd_list_free(head);
					red_list_free(pending_in);
					red_list_free(pending_out);
					intvec_free(out_codes);
					return (-1);
				}
			}
			else
			{
				char	*heredoc_content;
				char	*real_delim;
				int		expand_vars;

				if (op_code == 1)
					red_list_push_back(&pending_in, target, 0);
				else if (op_code == 3)
				{
					expand_vars = 1;
					if (strchr(target, '\'') || strchr(target, '"'))
						expand_vars = 0;
					real_delim = remove_quotes_str(target);
					if (!real_delim)
					{
						free(target);
						cmd_list_free(head);
						red_list_free(pending_in);
						red_list_free(pending_out);
						intvec_free(out_codes);
						return (-1);
					}
					heredoc_content = read_heredoc_input(real_delim, expand_vars, shell_env);
					free(real_delim);
					if (!heredoc_content)
					{
						free(target);
						cmd_list_free(head);
						red_list_free(pending_in);
						red_list_free(pending_out);
						intvec_free(out_codes);
						return (-1);
					}
					red_list_push_back(&pending_in, heredoc_content, 1);
					free(heredoc_content);
				}
				else if (op_code == 2)
					red_list_push_back(&pending_out, target, 0);
				else if (op_code == 4)
					red_list_push_back(&pending_out, target, 1);
			}
			free(target);
			continue ;
		}
		grouped = 0;
		cmd = read_token_grouped(line, &i, &grouped);
		if (!cmd)
		{
			i++;
			continue ;
		}
		expanded = expand_env_vars(cmd, shell_env);
		free(cmd);
		cmd = expanded;
		intvec_push(out_codes, 0);
		current_cmd = node_new(cmd);
		move_red_list_to_back(&current_cmd->infiles, &pending_in);
		move_red_list_to_back(&current_cmd->outfiles, &pending_out);
		cmd_list_append(&head, &tail, current_cmd);
		free(cmd);
		while (1)
		{
			size_t	j;

			j = i;
			skip_spaces(line, &j);
			if (!line[j])
			{
				i = j;
				break ;
			}
			op_match = match_op(line, j, &op_len, &op_code);
			if (op_match == -1)
			{
				cmd_list_free(head);
				red_list_free(pending_in);
				red_list_free(pending_out);
				intvec_free(out_codes);
				return (-1);
			}
			if (op_match == 1)
			{
				if (op_code == 5)
				{
					intvec_push(out_codes, op_code);
					i = j + op_len;
					current_cmd = NULL;
					break ;
				}
				intvec_push(out_codes, op_code);
				i = j + op_len;
				grouped = 0;
				target = read_token_grouped(line, &i, &grouped);
				if (!target)
				{
					fprintf(stderr, "Error: redirección sin destino.\n");
					cmd_list_free(head);
					red_list_free(pending_in);
					red_list_free(pending_out);
					intvec_free(out_codes);
					return (-1);
				}
				expanded = expand_env_vars(target, shell_env);
				free(target);
				target = expanded;
				if (add_redir_target(current_cmd, op_code, target, shell_env) == -1)
				{
					free(target);
					cmd_list_free(head);
					red_list_free(pending_in);
					red_list_free(pending_out);
					intvec_free(out_codes);
					return (-1);
				}
				free(target);
				continue ;
			}
			i = j;
			grouped = 0;
			tok = read_token_grouped(line, &i, &grouped);
			if (!tok)
				break ;
			expanded = expand_env_vars(tok, shell_env);
			free(tok);
			tok = expanded;
			str_list_push_back(&current_cmd->phrase, tok);
			free(tok);
		}
	}
	
	*out_list = head;
	return (0);
}