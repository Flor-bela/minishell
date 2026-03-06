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

static void	move_list_to_back(t_list **dst, t_list **src)
{
	t_list	*tail;

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
	n->prev = NULL;
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
		n->prev = *tail;
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
		str_list_free(h->infiles);
		str_list_free(h->outfiles);
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

static int	match_op(const char *s, size_t i, size_t *op_len, int *op_code)
{
	if (s[i] == '<')
	{
		if (s[i + 1] == '<')
		{
			if (s[i + 2] == '<')
			{
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
				return (-2);
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
			return (-3);
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

	if (grouped_out)
		*grouped_out = 0;
	skip_spaces(s, i);
	if (!s[*i])
		return (NULL);
	{
		int	op_match;

		op_match = match_op(s, *i, &op_len, &op_code);
		if (op_match != 0)
			return (NULL);
	}
	start = *i;
	quote = 0;
	while (s[*i])
	{
		if (quote == 0)
		{
			{
				int	op_match;

				op_match = match_op(s, *i, &op_len, &op_code);
				if (op_match != 0)
					break ;
			}
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

static int	add_redir_target(t_cmdnode *cmd, int op_code, const char *target)
{
	if (!cmd)
	{
		fprintf(stderr,
			"Error: redirección sin comando asociado.\n");
		return (-1);
	}
	if (op_code == 1 || op_code == 3)
		str_list_push_back(&cmd->infiles, target);
	else if (op_code == 2 || op_code == 4)
		str_list_push_back(&cmd->outfiles, target);
	return (0);
}

int	parse_line(const char *line, t_intvec *out_codes, t_cmdnode **out_list)
{
	size_t		i;
	size_t		op_len;
	int			op_code;
	t_cmdnode	*head;
	t_cmdnode	*tail;
	t_cmdnode	*current_cmd;
	t_list		*pending_in;
	t_list		*pending_out;
	int			grouped;
	char		*cmd;
	char		*target;
	char		*tok;

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
		if (match_op(line, i, &op_len, &op_code))
		{
			int	op_match;

			op_match = match_op(line, i, &op_len, &op_code);
			if (op_match == -1)
			{
				cmd_list_free(head);
				str_list_free(pending_in);
				str_list_free(pending_out);
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
					str_list_free(pending_in);
					str_list_free(pending_out);
					intvec_free(out_codes);
					return (-1);
				}
				if (current_cmd)
				{
					if (add_redir_target(current_cmd, op_code, target) == -1)
					{
						free(target);
						cmd_list_free(head);
						str_list_free(pending_in);
						str_list_free(pending_out);
						intvec_free(out_codes);
						return (-1);
					}
				}
				else
				{
					if (op_code == 1 || op_code == 3)
						str_list_push_back(&pending_in, target);
					else if (op_code == 2 || op_code == 4)
						str_list_push_back(&pending_out, target);
				}
				free(target);
				continue ;
			}
		}
		grouped = 0;
		cmd = read_token_grouped(line, &i, &grouped);
		if (!cmd)
		{
			i++;
			continue ;
		}
		intvec_push(out_codes, 0);
		current_cmd = node_new(cmd);
		move_list_to_back(&current_cmd->infiles, &pending_in);
		move_list_to_back(&current_cmd->outfiles, &pending_out);
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
			if (match_op(line, j, &op_len, &op_code))
			{
				int	op_match;

				op_match = match_op(line, j, &op_len, &op_code);
				if (op_match == -1)
				{
					fprintf(stderr,
						"Error: no se permiten mas de 2 redirecciones '<' seguidas.\n");
					cmd_list_free(head);
					str_list_free(pending_in);
					str_list_free(pending_out);
					intvec_free(out_codes);
					return (-1);
				}
				if (op_match == -2)
				{
					fprintf(stderr,
						"Error: no se permiten mas de 2 redirecciones '>' seguidas.\n");
					cmd_list_free(head);
					str_list_free(pending_in);
					str_list_free(pending_out);
					intvec_free(out_codes);
					return (-1);
				}
				if (op_match == -3)
				{
					fprintf(stderr,
						"Error: no se permiten 2 o mas pipes seguidos.\n");
					cmd_list_free(head);
					str_list_free(pending_in);
					str_list_free(pending_out);
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
						str_list_free(pending_in);
						str_list_free(pending_out);
						intvec_free(out_codes);
						return (-1);
					}
					if (add_redir_target(current_cmd, op_code, target) == -1)
					{
						free(target);
						cmd_list_free(head);
						str_list_free(pending_in);
						str_list_free(pending_out);
						intvec_free(out_codes);
						return (-1);
					}
					free(target);
					continue ;
				}
			}
			i = j;
			grouped = 0;
			tok = read_token_grouped(line, &i, &grouped);
			if (!tok)
				break ;
			str_list_push_back(&current_cmd->phrase, tok);
			free(tok);
		}
	}
	if (pending_in || pending_out)
	{
		fprintf(stderr, "Error: redirección sin comando asociado.\n");
		cmd_list_free(head);
		str_list_free(pending_in);
		str_list_free(pending_out);
		intvec_free(out_codes);
		return (-1);
	}
	*out_list = head;
	return (0);
}