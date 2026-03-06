#include "minishell.h"

int	parse_line(const char *line, t_intvec *out_codes, t_cmdnode **out_list);

static void	intvec_reset(t_intvec *v)
{
	v->data = NULL;
	v->len = 0;
	v->cap = 0;
}

static void	intvec_destroy(t_intvec *v)
{
	free(v->data);
	v->data = NULL;
	v->len = 0;
	v->cap = 0;
}

static void	str_list_destroy(t_list *lst)
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

static void	cmd_list_destroy(t_cmdnode *h)
{
	t_cmdnode	*nx;

	while (h)
	{
		nx = h->next;
		free(h->cmd);
		str_list_destroy(h->phrase);
		str_list_destroy(h->infiles);
		str_list_destroy(h->outfiles);
		free(h);
		h = nx;
	}
}

static void	print_int_array(const t_intvec *v)
{
	size_t	i;

	printf("codes: ");
	i = 0;
	while (i < v->len)
	{
		printf("%d", v->data[i]);
		if (i + 1 < v->len)
			printf(",");
		i++;
	}
	printf("\n");
}

static void	print_phrase_list(const t_list *lst)
{
	printf("[");
	while (lst)
	{
		printf("\"%s\"", lst->content ? (char *)lst->content : "");
		if (lst->next)
			printf(", ");
		lst = lst->next;
	}
	printf("]");
}
static void	print_str_list(const t_list *lst)
{
	printf("[");
	while (lst)
	{
		printf("\"%s\"", lst->content ? (char *)lst->content : "");
		if (lst->next)
			printf(", ");
		lst = lst->next;
	}
	printf("]");
}

static void	print_nodes(const t_cmdnode *h)
{
	int	idx;

	idx = 1;
	while (h)
	{
		printf("node%d: cmd=\"%s\" phrase=",
			idx, h->cmd ? h->cmd : "");
		print_str_list(h->phrase);
		printf(" infiles=");
		print_str_list(h->infiles);
		printf(" outfiles=");
		print_str_list(h->outfiles);
		printf(" prev=%s next=%s\n",
			h->prev ? "SI" : "NULL",
			h->next ? "SI" : "NULL");
		h = h->next;
		idx++;
	}
}

static void	update_quote_state_line(const char *line, char *quote_state)
{
	size_t	i;

	i = 0;
	while (line[i])
	{
		if (*quote_state == 0)
		{
			if (line[i] == '\'')
				*quote_state = '\'';
			else if (line[i] == '"')
				*quote_state = '"';
		}
		else if (line[i] == *quote_state)
			*quote_state = 0;
		i++;
	}
}

static char	*join_lines(char *base, const char *extra)
{
	size_t	len1;
	size_t	len2;
	char	*res;

	if (!base)
		return (strdup(extra));
	len1 = strlen(base);
	len2 = strlen(extra);
	res = (char *)malloc(len1 + 1 + len2 + 1);
	if (!res)
	{
		perror("malloc");
		free(base);
		return (NULL);
	}
	memcpy(res, base, len1);
	res[len1] = '\n';
	memcpy(res + len1 + 1, extra, len2);
	res[len1 + 1 + len2] = '\0';
	free(base);
	return (res);
}

static char	*read_full_command(void)
{
	char	*line;
	char	*full_line;
	size_t	cap;
	ssize_t	n;
	char	quote_state;

	full_line = NULL;
	quote_state = 0;
	while (1)
	{
		line = NULL;
		cap = 0;
		if (!full_line)
			printf("minishell$ ");
		else
			printf("> ");
		fflush(stdout);
		n = getline(&line, &cap, stdin);
		if (n <= 0)
		{
			free(line);
			free(full_line);
			return (NULL);
		}
		if (n > 0 && line[n - 1] == '\n')
			line[n - 1] = '\0';
		update_quote_state_line(line, &quote_state);
		full_line = join_lines(full_line, line);
		free(line);
		if (!full_line)
			return (NULL);
		if (quote_state == 0)
			break ;
	}
	return (full_line);
}

int	main(void)
{
	char		*line;
	t_intvec	codes;
	t_cmdnode	*list;

	while (1)
	{
		line = read_full_command();
		if (!line)
			break ;
		if (strcmp(line, "exit") == 0)
		{
			free(line);
			break ;
		}
		intvec_reset(&codes);
		list = NULL;
		if (parse_line(line, &codes, &list) == 0)
		{
			print_int_array(&codes);
			print_nodes(list);
		}
		else
			fprintf(stderr, "(parse error)\n");
		cmd_list_destroy(list);
		intvec_destroy(&codes);
		free(line);
	}
	return (0);
}