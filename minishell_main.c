/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell_main.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 23:52:06 by oscar             #+#    #+#             */
/*   Updated: 2026/03/23 18:39:56 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_sigint_received = 0;

void	handle_sigint(int sig)
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

	ft_memset(&sa, 0, sizeof(sa));
	sa.sa_handler = handle_sigint;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGINT, &sa, NULL);
	signal(SIGQUIT, SIG_IGN);
}

void	handle_sigint_heredoc(int sig)
{
	(void)sig;
	g_sigint_received = 1;
}

void	setup_signals_heredoc(void)
{
	struct sigaction	sa;

	ft_memset(&sa, 0, sizeof(sa));
	sa.sa_handler = handle_sigint_heredoc;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGINT, &sa, NULL);
	signal(SIGQUIT, SIG_IGN);
}

static void	intvec_reset(t_intvec *v)
{
	v->data = NULL;
	v->len = 0;
	v->cap = 0;
}

void	intvec_destroy(t_intvec *v)
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

void	cmd_list_destroy(t_cmdnode *h)
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

static void	update_quote_state_line(const char *line, char *quote)
{
	size_t	i;

	i = 0;
	while (line[i])
	{
		if (*quote == 0 && (line[i] == '\'' || line[i] == '"'))
			*quote = line[i];
		else if (*quote && line[i] == *quote)
			*quote = 0;
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
	res = malloc(len1 + len2 + 2);
	if (!res)
	{
		perror("malloc");
		free(base);
		return (NULL);
	}
	ft_memcpy(res, base, len1);
	res[len1] = '\n';
	ft_memcpy(res + len1 + 1, extra, len2);
	res[len1 + len2 + 1] = '\0';
	free(base);
	return (res);
}

static int	read_prompt_line(const char *prompt, char **out)
{
	char	*line;

	line = readline(prompt);
	if (!line)
	{
		*out = NULL;
		return (0);
	}
	*out = line;
	return (1);
}

static int	read_line_select(char **line, int has_prev)
{
	if (!has_prev)
		return (read_prompt_line("minishell$ ", line));
	return (read_prompt_line("> ", line));
}

static char	*append_line(char *full, char *line, char *quote)
{
	update_quote_state_line(line, quote);
	full = join_lines(full, line);
	free(line);
	return (full);
}

static char	*read_full_command(void)
{
	char	*line;
	char	*full;
	char	quote;
	int		status;

	full = NULL;
	quote = 0;
	while (1)
	{
		status = read_line_select(&line, full != NULL);
		if (status <= 0)
			return (full);
		full = append_line(full, line, &quote);
		if (!full)
			return (NULL);
		if (quote == 0)
			break ;
	}
	return (full);
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

static int	process_input(char **line)
{
	*line = read_full_command();
	if (!*line)
	{
		printf("exit\n");
		return (1);
	}
	if ((*line)[0] == '\0' || is_only_spaces(*line))
	{
		free(*line);
		return (2);
	}
	add_history(*line);
	return (0);
}

static int	handle_parse(char *line, t_intvec *codes,
	t_cmdnode **list, char ***env)
{
	int	exit_code;

	intvec_reset(codes);
	*list = NULL;
	if (parse_line(line, codes, list, *env) == 0)
	{
		exit_code = execute_cmds(list, env, line, codes);
		change_exit_value(ft_itoa(exit_code), env);
		return (0);
	}
	if (!g_sigint_received)
		change_exit_value("2", env);
	g_sigint_received = 0;
	return (1);
}

static void	cleanup_iteration(t_cmdnode *list,
	t_intvec *codes, char *line)
{
	cmd_list_destroy(list);
	intvec_destroy(codes);
	free(line);
}

static void	main_loop(char ***shell_env)
{
	char		*line;
	t_intvec	codes;
	t_cmdnode	*list;
	int			status;

	while (1)
	{
		status = process_input(&line);
		if (status == 1)
			break ;
		if (status == 2)
			continue ;
		handle_parse(line, &codes, &list, shell_env);
		cleanup_iteration(list, &codes, line);
	}
}

int	main(int argc, char **argv, char **envp)
{
	char	**shell_env;

	(void)argc;
	(void)argv;
	shell_env = dup_envp(envp);
	if (!shell_env)
		return (1);
	setup_signals_prompt();
	using_history();
	change_exit_value("127", &shell_env);
	main_loop(&shell_env);
	rl_clear_history();
	ft_free_malloc();
	free_envp_copy(shell_env);
	return (0);
}
