#include "minishell.h"

int	parse_line(const char *line, t_intvec *out_codes,
		t_cmdnode **out_list, char **shell_env);

int	g_sigint_received = 0;

static void	handle_sigint(int sig)
{
	(void)sig;
	write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
	rl_done = 1;
	g_sigint_received = 1;
}
 
static void	setup_signals(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;

	memset(&sa_int, 0, sizeof(sa_int));
	sa_int.sa_handler = handle_sigint;
	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = 0;
	sigaction(SIGINT, &sa_int, NULL);
	memset(&sa_quit, 0, sizeof(sa_quit));
	sa_quit.sa_handler = SIG_IGN;
	sigemptyset(&sa_quit.sa_mask);
	sa_quit.sa_flags = 0;
	sigaction(SIGQUIT, &sa_quit, NULL);
}

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

static void	red_list_destroy(t_list_red *lst)
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

static void	cmd_list_destroy(t_cmdnode *h)
{
	t_cmdnode	*nx;

	while (h)
	{
		nx = h->next;
		free(h->cmd);
		str_list_destroy(h->phrase);
		red_list_destroy(h->infiles);
		red_list_destroy(h->outfiles);
		free(h);
		h = nx;
	}
}
 /* 
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
} */
 
/* static void	print_str_list(const t_list *lst)
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
} */
/* static void	print_red_list(const t_list_red *lst)
{
	printf("[");
	while (lst)
	{
		printf("{\"%s\", doble=%d}",
			lst->content ? (char *)lst->content : "",
			lst->doble);
		if (lst->next)
			printf(", ");
		lst = lst->next;
	}
	printf("]");
} */
/* static void	print_nodes(const t_cmdnode *h)
{
	int	idx;

	idx = 1;
	while (h)
	{
		printf("node%d: cmd=\"%s\" phrase=",
			idx, h->cmd ? h->cmd : "");
		print_str_list(h->phrase);
		printf(" infiles=");
		print_red_list(h->infiles);
		printf(" outfiles=");
		print_red_list(h->outfiles);
		printf(" next=%s\n", h->next ? "SI" : "NULL");
		h = h->next;
		idx++;
	}
} */

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
		return (ft_strdup(extra));
	len1 = ft_strlen(base);
	len2 = ft_strlen(extra);
	res = (char *)malloc(len1 + 1 + len2 + 1);
	if (!res)
	{
		perror("malloc");
		free(base);
		return (NULL);
	}
	ft_memcpy(res, base, len1);
	res[len1] = '\n';
	ft_memcpy(res + len1 + 1, extra, len2);
	res[len1 + 1 + len2] = '\0';
	free(base);
	return (res);
}

/* static void	sigint_noninteractive(int sig)
{
	(void)sig;
	printf("\n");
	rl_replace_line("", 0);
	rl_redisplay();
	g_sigint_received = 1;
} */

/* static void sigint_noninteractive_setup(void)
{
	struct sigaction sa;

	sa.sa_handler = sigint_noninteractive;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
} */

static int	read_prompt_line(const char *prompt, char **out_line)
{
	char	*line;

	setup_signals();
	line = readline(prompt);
	printf("Exit the readline\n");
	signal(SIGINT, SIG_IGN);
	if (!line)
	{
		printf("Entered !line\n");
		*out_line = NULL;
		if (g_sigint_received)
			return (-1);
		return (0);
	}
	if (g_sigint_received == 1)
	{
		printf("Enetred g_sigint == 1\n");
		free(line);
		*out_line = NULL;
		g_sigint_received = 0;
		return (-1);
	}
	*out_line = line;
	return (1);
}

static char	*read_full_command(void)
{
	char	*line;
	char	*full_line;
	char	quote_state;
	int		status;

	full_line = NULL;
	quote_state = 0;
	while (1)
	{
		if (!full_line)
			status = read_prompt_line("minishell$ ", &line);
		else
			status = read_prompt_line("> ", &line);
		if (status == -1)
		{
			free(full_line);
			return (ft_strdup(""));
		}
		if (status == 0)
		{
			if (!full_line)
				return (NULL);
			continue ;
		}
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

static int	is_only_spaces(const char *s)
{
	size_t	i;

	i = 0;
	while (s[i])
	{
		if (!isspace((unsigned char)s[i]))
			return (0);
		i++;
	}
	return (1);
}

int	main(int argc, char **argv, char **envp)
{
	char		*line;
	t_intvec	codes;
	t_cmdnode	*list;
	char		**shell_env;
	int			exit_code;

	exit_code = 0;
	if ((argc = 0) && (argv == NULL))
	{
	}

	shell_env = dup_envp(envp);
	if (!shell_env)
		return (1);
	using_history();
	change_exit_value("127", &shell_env);
	while (1)
	{
		line = read_full_command();
		if (!line)
		{
			printf("exit\n");
			break ;
		}
		if (line[0] == '\0' || is_only_spaces(line))
		{
			free(line);
			continue ;
		}
		add_history(line);
		intvec_reset(&codes);
		list = NULL;
		if (parse_line(line, &codes, &list, shell_env) == 0)
		{
			//print_int_array(&codes);
			//print_nodes(list);
			exit_code = execute_cmds(&list, shell_env);
			printf("exit_code %d\n", exit_code);
			change_exit_value(ft_itoa(exit_code), &shell_env);
		}
		else
			write(2, "(parse error)\n", ft_strlen("(parse error)\n"));
		cmd_list_destroy(list);
		intvec_destroy(&codes);
		free(line);
		if (exit_code == -11)
		{
			printf("exit\n");
			break ;
		}
	}
	rl_clear_history();
	ft_free_malloc();
	free_envp_copy(shell_env);
	return (0);
}