#include "minishell.h"

int	parse_line(const char *line, t_intvec *out_codes,
		t_cmdnode **out_list, char **shell_env);

volatile sig_atomic_t g_sigint_received = 0;

void handle_sigint(int sig)
{
    (void)sig;
    g_sigint_received = 1;

    write(1, "\n", 1);
    rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

void	setup_signals_prompt(void)
{
	struct sigaction	sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = handle_sigint;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	signal(SIGQUIT, SIG_IGN);
}

void handle_sigint_heredoc(int sig)
{
	(void)sig;
	g_sigint_received = 1;
	//write(1, "\n", 1);
	//rl_redisplay();
}

void	setup_signals_heredoc(void)
{
	struct sigaction	sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = handle_sigint_heredoc;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0; // 🔥 MUY IMPORTANTE (sin SA_RESTART)

	sigaction(SIGINT, &sa, NULL);
	signal(SIGQUIT, SIG_IGN);
}

/*static void	setup_signals(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;

	memset(&sa_int, 0, sizeof(sa_int));
	sa_int.sa_handler = handle_sigint;
	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = 0;   // ❌ quita SA_RESTART
	sigaction(SIGINT, &sa_int, NULL);
	memset(&sa_quit, 0, sizeof(sa_quit));
	sa_quit.sa_handler = SIG_IGN;
	sigemptyset(&sa_quit.sa_mask);
	sa_quit.sa_flags = 0;
	sigaction(SIGQUIT, &sa_quit, NULL);
}*/

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

static void	print_red_list(const t_list_red *lst)
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
		print_red_list(h->infiles);
		printf(" outfiles=");
		print_red_list(h->outfiles);
		printf(" next=%s\n", h->next ? "SI" : "NULL");
		h = h->next;
		idx++;
	}
}
*/
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

static int	read_prompt_line(const char *prompt, char **out_line)
{
	char	*line;

	//g_sigint_received = 0; // 🔥 RESET AQUÍ

	line = readline(prompt);
	//dprintf(2,"DEBUG: readline main devuelve = %p (%s) %d\n", line, line, g_sigint_received);
	/* if (g_sigint_received && (!line || *line == '\0'))
	{
		free(line);
		*out_line = NULL;
		g_sigint_received = 0;
		return (-1);
	} */
	if (!line)
	{
		*out_line = NULL;
		return (0);
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
		{
			status = read_prompt_line("minishell$ ", &line);
			////printf("DEBUG read_prompt_line minishell devuelve cmds = %p\n", status);
		}
		else
		{
			status = read_prompt_line("> ", &line);
			////printf("DEBUG read_prompt_line > devuelve cmds = %p\n", status);
		}
		if (status == -1)
		{
			free(full_line);
			return (NULL);
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
	//int 		*num;
	

	exit_code = 0;
	(void)argc;
	(void)argv;
	shell_env = dup_envp(envp);
	if (!shell_env)
		return (1);
	setup_signals_prompt();
	using_history();
	change_exit_value("127", &shell_env);
	while (1)
	{
		line = NULL;
		//num = 0;
		//write(2, "DEBUG: antes de parsear\n", 24);
		line = read_full_command();
		////printf("DEBUG despues de parsear\n");
		/*if (g_sigint_received)
		{
			//printf("DEBUG Ctrl+C detectado → salto iteración\n");
			g_sigint_received = 0;
			free(line);
			continue;
		}*/
		
		if (!line)
		{
			//if (g_sigint_received && *num !=1)
			/* if (g_sigint_received)
			{
				g_sigint_received = 0;

				
				rl_redisplay();
				continue;
			} */

			printf("exit\n");
			break;
		}
		
		if (line[0] == '\0' || is_only_spaces(line))
		{
			free(line);
			continue;
		}
		add_history(line);
		intvec_reset(&codes);
		list = NULL;
		if (parse_line(line, &codes, &list, shell_env) == 0)
		{
			//print_red_list(list->infiles);
			/*if (g_sigint_received)
			{
				//printf("DEBUG heredoc cancelado → no ejecutar\n");
				
				g_sigint_received = 0;
				cmd_list_destroy(list);
				intvec_destroy(&codes);
				free(line);
				continue;
			}*/
			//print_int_array(&codes);
			//print_nodes(list);
			exit_code = execute_cmds(&list, &shell_env);
			//printf("DEBUG salio de execute_cmds:\n");	
			change_exit_value(ft_itoa(exit_code), &shell_env);
		}
		else
		{
			if (!g_sigint_received)
			{
			//	write(2, "minishell: syntax error\n", 24);
				change_exit_value("2", &shell_env);
			}
			g_sigint_received = 0;
		}
			//printf("DEBUG free-1\n");	
		cmd_list_destroy(list);
			//printf("DEBUG free0\n");	
		intvec_destroy(&codes);
		free(line);
		//printf("DEBUG free1\n");	
		/* if (exit_code == -11)  // ver esto!!!! como saber que entró a exit!
		{
			printf("exit\n");
			break ;
		} */
	}
	rl_clear_history();
	ft_free_malloc();
	free_envp_copy(shell_env);
	return (0);
}