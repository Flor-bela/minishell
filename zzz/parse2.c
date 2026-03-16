/*#include "../minishell.h"

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

static char	*dup_range(const char *s, size_t a, size_t b)
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
	memcpy(out, s + a, n);
	out[n] = 0;
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

/* ---------- ENV EXPANSION ---------- */

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

static char	*append_char_dyn(char *base, char c)
{
	size_t	len;
	char	*res;

	if (!base)
		return (append_char_new(c));
	len = strlen(base);
	res = malloc(len + 2);
	if (!res)
	{
		perror("malloc");
		free(base);
		exit(1);
	}
	memcpy(res, base, len);
	res[len] = c;
	res[len + 1] = 0;
	free(base);
	return (res);
}

static char	*append_str_new(const char *add)
{
	char	*res;

	res = strdup(add);
	if (!res)
	{
		perror("strdup");
		exit(1);
	}
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
		return (append_str_new(add));
	len1 = strlen(base);
	len2 = strlen(add);
	res = malloc(len1 + len2 + 1);
	if (!res)
	{
		perror("malloc");
		free(base);
		exit(1);
	}
	memcpy(res, base, len1);
	memcpy(res + len1, add, len2);
	res[len1 + len2] = 0;
	free(base);
	return (res);
}

static char	*expand_variable(const char *token, size_t *i,
	char *res, char **shell_env)
{
	size_t	start;
	char	*var_name;
	char	*value;

	if (!token[*i])
		return (append_char_dyn(res, '$'));
	if (token[*i] == '?')
	{
		value = get_env_value(shell_env, "return_value");
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
	value = get_env_value(shell_env, var_name);
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

static char	*expand_env_vars(const char *token, char **shell_env)
{
	size_t	i;
	char	*res;
	char	quote;

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
		quote = update_quote(quote, token[i]);
		if (token[i] == '$' && quote != '\'')
		{
			i++;
			res = expand_variable(token, &i, res, shell_env);
			continue ;
		}
		res = append_char_dyn(res, token[i]);
		i++;
	}
	return (res);
}

/* ---------- OPERATORS ---------- */

static int	handle_less(const char *s, size_t i, size_t *len, int *code)
{
	if (s[i + 1] == '<')
	{
		if (s[i + 2] == '<')
		{
			fprintf(stderr,
				"Error: no se permiten mas de 2 redirecciones '<' seguidas.\n");
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
			fprintf(stderr,
				"Error: no se permiten mas de 2 redirecciones '>' seguidas.\n");
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
		fprintf(stderr, "Error: no se permiten 2 o mas pipes seguidos.\n");
		return (-1);
	}
	*len = 1;
	*code = 5;
	return (1);
}

static int	match_op(const char *s, size_t i, size_t *len, int *code)
{
	if (s[i] == '<')
		return (handle_less(s, i, len, code));
	if (s[i] == '>')
		return (handle_greater(s, i, len, code));
	if (s[i] == '|')
		return (handle_pipe(s, i, len, code));
	return (0);
}

static void	update_token_quote(const char *s, size_t *i,
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

static int	read_token_loop(const char *s, size_t *i,
	char *quote, int *grouped_out)
{
	size_t	op_len;
	int		op_code;

	while (s[*i])
	{
		if (*quote == 0)
		{
			if (match_op(s, *i, &op_len, &op_code) != 0
				|| isspace((unsigned char)s[*i]))
				break ;
			if (s[*i] == '\'' || s[*i] == '"')
				update_token_quote(s, i, quote, grouped_out);
			else
				(*i)++;
		}
		else
			update_token_quote(s, i, quote, grouped_out);
	}
	return (0);
}

static char	*read_token_grouped(const char *s, size_t *i, int *grouped_out)
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
	if (quote != 0)
	{
		fprintf(stderr, "Error: comillas sin cerrar.\n");
		return (NULL);
	}
	return (dup_range(s, start, *i));
}

/* ---------- PARSER ---------- */

static void	parser_init(t_parser *p, t_cmdnode **out_list)
{
	p->i = 0;
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

static int	handle_heredoc(t_cmdnode *cmd, const char *target, char **shell_env)
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
	heredoc_content = read_heredoc_input(real_delim, expand_vars, shell_env);
	free(real_delim);
	if (!heredoc_content)
		return (-1);
	red_list_push_back(&cmd->infiles, heredoc_content, 1);
	free(heredoc_content);
	return (0);
}

static int	add_redir_target(t_cmdnode *cmd, int op_code,
	const char *target, char **shell_env)
{
	if (!cmd)
	{
		fprintf(stderr, "Error: redirección sin comando asociado.\n");
		return (-1);
	}
	if (op_code == 1)
		red_list_push_back(&cmd->infiles, target, 0);
	else if (op_code == 3)
		return (handle_heredoc(cmd, target, shell_env));
	else if (op_code == 2)
		red_list_push_back(&cmd->outfiles, target, 0);
	else if (op_code == 4)
		red_list_push_back(&cmd->outfiles, target, 1);
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

static int	store_pending_redir(t_parser *p, int op_code,
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

static char	*get_redir_target(const char *line, size_t *i, char **env)
{
	char	*target;
	char	*expanded;
	int		grouped;

	grouped = 0;
	target = read_token_grouped(line, i, &grouped);
	if (!target)
	{
		fprintf(stderr, "Error: redirección sin destino.\n");
		return (NULL);
	}
	expanded = expand_env_vars(target, env);
	free(target);
	return (expanded);
}

static int	store_operator_target(t_parser *p,
	char *target, char **env)
{
	if (p->current_cmd)
		return (add_redir_target(p->current_cmd, p->op_code, target, env));
	return (store_pending_redir(p, p->op_code, target, env));
}

static int	handle_operator(const char *line, t_parser *p,
	t_intvec *codes, char **env)
{
	char	*target;

	intvec_push(codes, p->op_code);
	p->i += p->op_len;
	if (p->op_code == 5)
	{
		p->current_cmd = NULL;
		return (0);
	}
	target = get_redir_target(line, &p->i, env);
	if (!target)
		return (-1);
	if (store_operator_target(p, target, env))
	{
		free(target);
		return (-1);
	}
	free(target);
	return (0);
}

static int	add_command_token(const char *line, t_parser *p, char **env)
{
	int		grouped;
	char	*tok;
	char	*expanded;

	grouped = 0;
	tok = read_token_grouped(line, &p->i, &grouped);
	if (!tok)
		return (1);
	expanded = expand_env_vars(tok, env);
	free(tok);
	str_list_push_back(&p->current_cmd->phrase, expanded);
	free(expanded);
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

static int	handle_command(const char *line, t_parser *p,
	t_intvec *codes, char **env)
{
	char	*cmd;
	char	*expanded;
	int		grouped;

	grouped = 0;
	cmd = read_token_grouped(line, &p->i, &grouped);
	if (!cmd)
		return (0);
	expanded = expand_env_vars(cmd, env);
	free(cmd);
	cmd = expanded;
	intvec_push(codes, 0);
	p->current_cmd = node_new(cmd);
	move_red_list_to_back(&p->current_cmd->infiles, &p->pending_in);
	move_red_list_to_back(&p->current_cmd->outfiles, &p->pending_out);
	cmd_list_append(&p->head, &p->tail, p->current_cmd);
	free(cmd);
	return (fill_command_tail(line, p, codes, env));
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
	if (p->pending_in || p->pending_out)
	{
		fprintf(stderr, "Error: redirección sin comando asociado.\n");
		return (-1);
	}
	return (0);
}

int	parse_line(const char *line, t_intvec *codes,
	t_cmdnode **out_list, char **env)
{
	t_parser	p;

	parser_init(&p, out_list);
	if (parse_loop(line, &p, codes, env) == -1)
		return (parser_error(&p, codes));
	*out_list = p.head;
	return (0);
}
*/