## File Structure
```
copiaMinishell/
├── built_ins
│   ├── builtin_cd.c
│   ├── provisionaryFunctions.h
│   └── provisionaryFunctions2.c
├── execution
│   ├── close_fds.c
│   ├── execution.c
│   ├── find_chr.c
│   ├── get_cmd_path.c
│   ├── is_built_in.c
│   └── strchr_modif.c
├── ft_utils
│   ├── ft_atoi.c
│   ├── ft_isdigit.c
│   ├── ft_itoa.c
│   ├── ft_lstadd_back.c
│   ├── ft_lstnew.c
│   ├── ft_mallocs_list.c
│   ├── ft_memcpy.c
│   ├── ft_memset.c
│   ├── ft_realloc.c
│   ├── ft_split.c
│   ├── ft_strchr.c
│   ├── ft_strcmp.c
│   ├── ft_strdup.c
│   ├── ft_strjoin_modif.c
│   ├── ft_strjoin.c
│   ├── ft_strlcpy.c
│   ├── ft_strlen.c
│   ├── ft_strncmp.c
│   └── ft_strndup.c
├── parse
│   ├── parser_cmd_utils.c
│   ├── parser_command.c
│   ├── parser_core.c
│   ├── parser_core2.c
│   ├── parser_expand_utils.c
│   ├── parser_expand.c
│   ├── parser_list_redir.c
│   ├── parser_list_utils.c
│   ├── parser_operator_handler.c
│   ├── parser_operator.c
│   ├── parser_redir.c
│   ├── parser_token_utils.c
│   ├── parser_token.c
│   └── parser.h
├── minishell_main.c
└── minishell.h
```

### `built_ins/builtin_cd.c`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/17 12:44:37 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/21 04:05:03 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "provisionaryFunctions.h"

static int	update_env(char **envp, char *key, char *path)
{
	size_t	i;
	size_t	len;
	char	*new_entry;
	char	*final_entry;

	if (envp == NULL || key == NULL || path == NULL)
		return (1);
	len = ft_strlen(key);
	i = 0;
	while (envp[i] != NULL)
	{
		if (ft_strncmp(envp[i], key, len) == 0 && envp[i][len] == '=')
		{
			free(envp[i]);
			new_entry = ft_strjoin(key, "=");
			final_entry = ft_strjoin(new_entry, path);
			free(new_entry);
			envp[i] = final_entry;
			return (0);
		}
		i++;
	}
	return (0);
}

static char	*cd_get_path(t_list *args, char **envp, char *oldpwd)
{
	char	*path;

	if (!args)
	{
		path = get_env_value(envp, "HOME");
		if (!path)
		{
			write(2, "minishell: cd: HOME not set\n", 29);
			free(oldpwd);
			return (NULL);
		}
	}
	else
		path = args->content;
	return (path);
}

static int	cd_change_dir(char *path, char *oldpwd)
{
	if (chdir(path) != 0)
	{
		perror("minishell: cd");
		free(oldpwd);
		return (1);
	}
	return (0);
}

static void	cd_update_env(char **envp, char *oldpwd)
{
	char	*newpwd;

	newpwd = getcwd(NULL, 0);
	if (!newpwd)
		newpwd = ft_strdup("");
	update_env(envp, "OLDPWD", oldpwd);
	update_env(envp, "PWD", newpwd);
	free(oldpwd);
	free(newpwd);
}

int	builtin_cd(t_list *args, char **envp)
{
	char	*path;
	char	*oldpwd;

	oldpwd = NULL;
	if (args && args->next)
	{
		write(2, "minishell: cd: too many arguments\n", 35);
		return (1);
	}
	oldpwd = getcwd(NULL, 0);
	if (!envp)
		return (1);
	if (!oldpwd)
		oldpwd = ft_strdup("");
	path = cd_get_path(args, envp, oldpwd);
	if (!path)
		return (1);
	if (cd_change_dir(path, oldpwd))
		return (1);
	cd_update_env(envp, oldpwd);
	return (0);
}

```

### `built_ins/provisionaryFunctions.h`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   provisionaryFunctions.h                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 11:12:35 by oscar             #+#    #+#             */
/*   Updated: 2026/03/21 05:19:23 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PROVISIONARYFUNCTIONS_H
# define PROVISIONARYFUNCTIONS_H

# include "../minishell.h"

typedef struct s_heredoc_ctx
{
	char		*line;
	char		*content;
	const char	*delimiter;
	int			expand;
	char		**env;
	int			status;
}	t_heredoc_ctx;

typedef struct s_heredoc_fork
{
	char		*delimiter;
	int			expand;
	char		**env;
	t_cmdnode	**data;
	char		*line;
	t_intvec	*codes;
}	t_heredoc_fork;

char	**dup_envp(char **envp);
char	*get_env_value(char **envp, const char *key);
void	free_envp_copy(char **envp);

char	*remove_quotes_str(const char *str);
char	*read_heredoc_input(const char *delimiter,
			int expand_vars, char **shell_env);

int		builtin_echo(t_list *args);
int		builtin_pwd(void);
int		builtin_export(t_list *args, char ***envp);
int		builtin_unset(t_list *args, char ***envp);
int		builtin_env(char **envp);
int		builtin_cd(t_list *args, char **envp);
size_t	env_len(char **envp);

void	change_exit_value(char *args, char ***envp);
char	*heredoc_with_fork(t_heredoc_fork *ctx);

#endif
```

### `built_ins/provisionaryFunctions2.c`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   provisionaryFunctions2.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 11:12:04 by oscar             #+#    #+#             */
/*   Updated: 2026/03/21 11:30:59 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "provisionaryFunctions.h"

size_t	env_len(char **envp)
{
	size_t	i;

	i = 0;
	while (envp && envp[i])
		i++;
	return (i);
}

static char	*ms_strdup(const char *s)
{
	size_t	len;
	char	*dup;

	if (!s)
		return (NULL);
	len = ft_strlen(s);
	dup = (char *)malloc(len + 1);
	if (!dup)
		return (NULL);
	ft_memcpy(dup, s, len);
	dup[len] = '\0';
	return (dup);
}

static int	is_valid_identifier(const char *s)
{
	size_t	i;

	if (!s || !s[0])
		return (0);
	if (!(isalpha((unsigned char)s[0]) || s[0] == '_'))
		return (0);
	i = 1;
	while (s[i] && s[i] != '=')
	{
		if (!(isalnum((unsigned char)s[i]) || s[i] == '_'))
			return (0);
		i++;
	}
	return (1);
}

static size_t	key_len(const char *s)
{
	size_t	i;

	i = 0;
	while (s[i] && s[i] != '=')
		i++;
	return (i);
}

static int	find_env_index(char **envp, const char *key)
{
	size_t	i;
	size_t	klen;

	if (!envp || !key)
		return (-1);
	klen = key_len(key);
	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], key, klen) == 0
			&& (envp[i][klen] == '=' || envp[i][klen] == '\0'))
			return ((int)i);
		i++;
	}
	return (-1);
}

static int	replace_env_value(char **envp, int idx, const char *new_value)
{
	char	*dup;

	dup = ms_strdup(new_value);
	if (!dup)
		return (1);
	free(envp[idx]);
	envp[idx] = dup;
	return (0);
}

static char	**copy_env(char **env, size_t len)
{
	size_t	i;
	char	**new_env;

	new_env = malloc(sizeof(char *) * (len + 2));
	if (!new_env)
		return (NULL);
	i = 0;
	while (i < len)
	{
		new_env[i] = env[i];
		i++;
	}
	return (new_env);
}

static int	append_env_value(char ***envp, const char *new_value)
{
	size_t	len;
	char	**new_env;

	len = env_len(*envp);
	new_env = copy_env(*envp, len);
	if (!new_env)
		return (1);
	new_env[len] = ms_strdup(new_value);
	if (!new_env[len])
	{
		free(new_env);
		return (1);
	}
	new_env[len + 1] = NULL;
	free(*envp);
	*envp = new_env;
	return (0);
}

static char	**alloc_env_minus_one(size_t len)
{
	return (malloc(sizeof(char *) * len));
}

static int	remove_env_index(char ***envp, int idx)
{
	size_t	len;
	size_t	i;
	size_t	j;
	char	**new_env;

	if (!envp || !*envp || idx < 0)
		return (0);
	len = env_len(*envp);
	new_env = alloc_env_minus_one(len);
	if (!new_env)
		return (1);
	i = 0;
	j = 0;
	while (i < len)
	{
		if ((int)i != idx)
			new_env[j++] = (*envp)[i];
		else
			free((*envp)[i]);
		i++;
	}
	new_env[j] = NULL;
	free(*envp);
	*envp = new_env;
	return (0);
}

static void	print_export_env(char **envp)
{
	size_t	i;

	i = 0;
	while (envp && envp[i])
	{
		printf("declare -x %s\n", envp[i]);
		i++;
	}
}

char	**dup_envp(char **envp)
{
	size_t	len;
	char	**copy;
	size_t	i;

	len = env_len(envp);
	copy = (char **)malloc(sizeof(char *) * (len + 1));
	if (!copy)
		return (NULL);
	i = 0;
	while (i < len)
	{
		copy[i] = ms_strdup(envp[i]);
		if (!copy[i])
		{
			while (i > 0)
				free(copy[--i]);
			free(copy);
			return (NULL);
		}
		i++;
	}
	copy[len] = NULL;
	return (copy);
}

char	*get_env_value(char **envp, const char *key)
{
	size_t	i;
	size_t	len;

	if (!envp || !key)
		return (NULL);
	len = ft_strlen(key);
	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], key, len) == 0 && envp[i][len] == '=')
			return (envp[i] + len + 1);
		i++;
	}
	return (NULL);
}

void	free_envp_copy(char **envp)
{
	size_t	i;

	if (!envp)
		return ;
	i = 0;
	while (envp[i])
	{
		free(envp[i]);
		i++;
	}
	free(envp);
}

static int	is_n_flag(char *s)
{
	int	i;

	if (!s || s[0] != '-' || s[1] != 'n')
		return (0);
	i = 1;
	while (s[i] == 'n')
		i++;
	if (s[i] != '\0')
		return (0);
	return (1);
}

static void	print_without_outer_quotes(char *str)
{
	size_t	i;
	char	quote;

	if (!str)
		return ;
	i = 0;
	quote = 0;
	while (str[i])
	{
		if ((str[i] == '"' || str[i] == '\''))
		{
			if (quote == 0)
				quote = str[i];
			else if (quote == str[i])
				quote = 0;
			else
				write(1, &str[i], 1);
		}
		else
			write(1, &str[i], 1);
		i++;
	}
}

static char	*join_and_free(char *s1, const char *s2)
{
	size_t	len1;
	size_t	len2;
	char	*res;

	if (!s1)
	{
		res = ft_strdup(s2);
		if (!res)
			return (NULL);
		return (res);
	}
	len1 = ft_strlen(s1);
	len2 = ft_strlen(s2);
	res = malloc(len1 + len2 + 1);
	if (!res)
	{
		free(s1);
		return (NULL);
	}
	ft_memcpy(res, s1, len1);
	ft_memcpy(res + len1, s2, len2);
	res[len1 + len2] = '\0';
	free(s1);
	return (res);
}

static void	handle_quote_char(const char *str, size_t *j,
								char *quote, char *res)
{
	if (*quote == 0)
		*quote = str[0];
	else if (*quote == str[0])
		*quote = 0;
	else
		res[(*j)++] = str[0];
}

char	*remove_quotes_str(const char *str)
{
	size_t	i;
	size_t	j;
	char	quote;
	char	*res;

	if (!str)
		return (NULL);
	res = malloc(ft_strlen(str) + 1);
	if (!res)
		return (NULL);
	i = 0;
	j = 0;
	quote = 0;
	while (str[i])
	{
		if (str[i] == '"' || str[i] == '\'')
			handle_quote_char(&str[i], &j, &quote, res);
		else
			res[j++] = str[i];
		i++;
	}
	res[j] = '\0';
	return (res);
}

static int	is_var_start_local(char c)
{
	return (isalpha((unsigned char)c) || c == '_');
}

static int	is_var_char_local(char c)
{
	return (isalnum((unsigned char)c) || c == '_');
}

static char	*append_char_local(char *base, char c)
{
	size_t	len;
	char	*res;

	if (!base)
	{
		res = malloc(2);
		if (!res)
			return (NULL);
		res[0] = c;
		res[1] = '\0';
		return (res);
	}
	len = ft_strlen(base);
	res = malloc(len + 2);
	if (!res)
	{
		free(base);
		return (NULL);
	}
	ft_memcpy(res, base, len);
	res[len] = c;
	res[len + 1] = '\0';
	free(base);
	return (res);
}

static char	*append_str_local(char *base, const char *add)
{
	size_t	len1;
	size_t	len2;
	char	*res;

	if (!add)
		add = "";
	if (!base)
		return (ft_strdup(add));
	len1 = ft_strlen(base);
	len2 = ft_strlen(add);
	res = malloc(len1 + len2 + 1);
	if (!res)
	{
		free(base);
		return (NULL);
	}
	ft_memcpy(res, base, len1);
	ft_memcpy(res + len1, add, len2);
	res[len1 + len2] = '\0';
	free(base);
	return (res);
}

static char	*get_var_value(const char *line, size_t *i, char **env)
{
	size_t	start;
	char	*name;
	char	*value;

	start = *i;
	while (line[*i] && is_var_char_local(line[*i]))
		(*i)++;
	name = ft_strndup(line + start, *i - start);
	if (!name)
		return (NULL);
	value = get_env_value(env, name);
	free(name);
	return (value);
}

static char	*expand_dollar(const char *line, size_t *i,
							char *res, char **shell_env)
{
	char	*value;

	if (!line[*i])
		return (append_char_local(res, '$'));
	if (line[*i] == '?')
	{
		value = get_env_value(shell_env, "cp_envp");
		if (value)
			res = append_str_local(res, value);
		(*i)++;
		return (res);
	}
	if (!is_var_start_local(line[*i]))
		return (append_char_local(res, '$'));
	value = get_var_value(line, i, shell_env);
	if (value)
		res = append_str_local(res, value);
	return (res);
}

static char	*expand_heredoc_line(const char *line, char **shell_env)
{
	size_t	i;
	char	*res;

	i = 0;
	res = ft_strdup("");
	if (!res)
		return (NULL);
	while (line[i])
	{
		if (line[i] == '$')
		{
			i++;
			res = expand_dollar(line, &i, res, shell_env);
		}
		else
		{
			res = append_char_local(res, line[i]);
			i++;
		}
		if (!res)
			return (NULL);
	}
	return (res);
}

static char	*get_next_line(int fd)
{
	char	*buf;
	int		i;
	char	c;

	buf = malloc(1024);
	i = 0;
	if (!buf)
		return (NULL);
	while (read(fd, &c, 1) > 0)
	{
		buf[i++] = c;
		if (c == '\n')
			break ;
	}
	if (i == 0)
	{
		free(buf);
		return (NULL);
	}
	buf[i] = '\0';
	return (buf);
}

static int	check_sigint(char *line, char *content)
{
	if (g_sigint_received)
	{
		g_sigint_received = 0;
		free(line);
		free(content);
		return (1);
	}
	return (0);
}

static void	trim_newline(char *line)
{
	size_t	len;

	len = ft_strlen(line);
	if (len > 0 && line[len - 1] == '\n')
		line[len - 1] = '\0';
}

static char	*expand_line_if_needed(char *line, int expand, char **env)
{
	char	*expanded;

	if (!expand)
		return (line);
	expanded = expand_heredoc_line(line, env);
	free(line);
	return (expanded);
}

static char	*append_line_content(char *content, char *line)
{
	char	*tmp;

	tmp = join_and_free(content, line);
	if (!tmp)
		return (NULL);
	content = tmp;
	tmp = join_and_free(content, "\n");
	if (!tmp)
		return (NULL);
	return (tmp);
}

static int	process_heredoc_line(t_heredoc_ctx *ctx)
{
	char	*tmp;

	trim_newline(ctx->line);
	if (ft_strcmp(ctx->line, ctx->delimiter) == 0)
	{
		free(ctx->line);
		return (1);
	}
	ctx->line = expand_line_if_needed(ctx->line,
			ctx->expand, ctx->env);
	if (!*ctx->line)
	{
		free(ctx->line);   // 🔥 FIX
		return (-1);
	}
	tmp = append_line_content(ctx->content, ctx->line);
	free(ctx->line);
	if (!tmp)
		return (-1);
	ctx->content = tmp;
	return (0);
}

static int	init_heredoc_ctx(t_heredoc_ctx *ctx,
	const char *delimiter, int expand, char **env)
{
	ctx->content = ft_strdup("");
	if (!ctx->content)
		return (0);
	ctx->delimiter = delimiter;
	ctx->expand = expand;
	ctx->env = env;
	//free((char *delimiter)); ///
	return (1);
}

static int	heredoc_loop(t_heredoc_ctx *ctx)
{
	while (1)
	{
		write(1, "> ", 2);
		ctx->line = get_next_line(STDIN_FILENO);
		if (check_sigint(ctx->line, ctx->content))
			return (0);
		if (!ctx->line)
			break ;
		ctx->status = process_heredoc_line(ctx);
		if (ctx->status == 1)
			break ;
		if (ctx->status == -1)
			return (0);
	}
	return (1);
}

char	*read_heredoc_input(const char *delimiter,
	int expand_vars, char **shell_env)
{
	t_heredoc_ctx	ctx;

	g_sigint_received = 0;
	setup_signals_heredoc();
	if (!init_heredoc_ctx(&ctx, delimiter, expand_vars, shell_env))
		return (NULL);
	if (!heredoc_loop(&ctx))
	{
		free(ctx.content);
		return (NULL);
	}
	return (ctx.content);
}

static char	*append_buffer(char *result, char *buffer)
{
	char	*tmp;

	tmp = join_and_free(result, buffer);
	if (!tmp)
	{
		free(result);
		return (NULL);
	}
	return (tmp);
}

char	*read_fd_all(int fd)
{
	char	buffer[1025];
	char	*result;
	ssize_t	bytes;

	result = ft_strdup("");
	if (!result)
		return (NULL);
	while (1)
	{
		bytes = read(fd, buffer, 1024);
		if (bytes < 0)
		{
			free(result);
			return (NULL);
		}
		if (bytes == 0)
			break ;
		buffer[bytes] = '\0';
		result = append_buffer(result, buffer);
		if (!result)
			return (NULL);
	}
	return (result);
}

static void	heredoc_child(t_heredoc_fork *ctx, int fd[2])
{
	char	*content;

	close(fd[0]);
	content = read_heredoc_input(ctx->delimiter, ctx->expand, ctx->env);
	if (!content)
	{
		close(fd[1]);
		ft_free_malloc();
		free_envp_copy(ctx->env);
		cmd_list_destroy(*ctx->data);
		free(ctx->line);
		intvec_destroy(ctx->codes);
		exit(130);
	}
	write(fd[1], content, ft_strlen(content));
	free(content);
	close(fd[1]);
	ft_free_malloc();
	free_envp_copy(ctx->env);
	cmd_list_destroy(*ctx->data);
	free(ctx->line);
	intvec_destroy(ctx->codes);
	exit(0);
}

static int	check_heredoc_signal(int status, char ***env, int fd)
{
	if (g_sigint_received == 1
		|| (WIFEXITED(status) && WEXITSTATUS(status) == 130)
		|| (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT))
	{
		close(fd);
		g_sigint_received = 1;
		change_exit_value("130", env);
		return (1);
	}
	return (0);
}

char	*heredoc_with_fork(t_heredoc_fork *ctx)
{
	int		fd[2];
	pid_t	pid;
	char	*content;
	int		status;

	if (pipe(fd) == -1)
		return (NULL);
	pid = fork();
	if (pid == -1)
		return (NULL);
	if (pid == 0)
		heredoc_child(ctx, fd);
	close(fd[1]);
	waitpid(pid, &status, 0);
	if (check_heredoc_signal(status, &ctx->env, fd[0]))
		return (NULL);
	content = read_fd_all(fd[0]);
	close(fd[0]);
	return (content);
}

int	builtin_echo(t_list *args)
{
	t_list	*curr;
	int		newline;

	curr = args;
	newline = 1;
	while (curr && is_n_flag((char *)curr->content))
	{
		newline = 0;
		curr = curr->next;
	}
	while (curr)
	{
		print_without_outer_quotes((char *)curr->content);
		if (curr->next)
			write(1, " ", 1);
		curr = curr->next;
	}
	if (newline)
		write(1, "\n", 1);
	return (0);
}

int	builtin_pwd(void)
{
	char	*cwd;
	size_t	size;

	size = 128;
	cwd = NULL;
	while (1)
	{
		cwd = (char *)malloc(size);
		if (!cwd)
			return (1);
		if (getcwd(cwd, size))
			break ;
		free(cwd);
		cwd = NULL;
		if (errno != ERANGE)
			return (1);
		size *= 2;
	}
	printf("%s\n", cwd);
	free(cwd);
	return (0);
}

static int	process_export_update(char *arg, char ***envp, int idx)
{
	if (idx >= 0)
	{
		if (replace_env_value(*envp, idx, arg) != 0)
		{
			change_exit_value("1", envp);
			return (1);
		}
	}
	else
	{
		if (append_env_value(envp, arg) != 0)
		{
			change_exit_value("1", envp);
			return (1);
		}
	}
	return (0);
}

static int	process_export_arg(char *arg, char ***envp)
{
	int	idx;

	if (!is_valid_identifier(arg))
	{
		write(2, "minishell: export: `", 20);
		write(2, arg, ft_strlen(arg));
		write(2, "': not a valid identifier\n", 26);
		change_exit_value("1", envp);
		return (1);
	}
	if (!strchr(arg, '='))
		return (0);
	idx = find_env_index(*envp, arg);
	return (process_export_update(arg, envp, idx));
}

int	builtin_export(t_list *args, char ***envp)
{
	t_list	*curr;
	char	*arg;

	if (!args)
	{
		print_export_env(*envp);
		return (0);
	}
	curr = args;
	while (curr)
	{
		if (!curr->content)
		{
			curr = curr->next;
			continue ;
		}
		arg = (char *)curr->content;
		if (process_export_arg(arg, envp))
			return (1);
		curr = curr->next;
	}
	return (0);
}

static int	process_unset_arg(char *arg, char ***envp)
{
	int	idx;

	if (!is_valid_identifier(arg))
	{
		write(2, "minishell: unset: `", 19);
		write(2, arg, ft_strlen(arg));
		write(2, "': not a valid identifier\n", 26);
		change_exit_value("1", envp);
		return (1);
	}
	idx = find_env_index(*envp, arg);
	if (idx >= 0)
	{
		if (remove_env_index(envp, idx) != 0)
			return (-1);
	}
	return (0);
}

int	builtin_unset(t_list *args, char ***envp)
{
	t_list	*curr;
	char	*arg;
	int		status;

	curr = args;
	while (curr)
	{
		arg = (char *)curr->content;
		status = process_unset_arg(arg, envp);
		if (status == -1)
			return (1);
		curr = curr->next;
	}
	return (0);
}

int	builtin_env(char **envp)
{
	size_t	i;

	i = 0;
	while (envp && envp[i])
	{
		printf("%s\n", envp[i]);
		i++;
	}
	return (0);
}

static void	exit_cleanup(t_exit_ctx *ctx)
{
	ft_free_malloc();
	free_envp_copy(*ctx->envp);
	cmd_list_destroy(*ctx->data);
	free(ctx->line);
	intvec_destroy(ctx->codes);
}

static int	count_args(t_list *args)
{
	int		i;
	t_list	*tmp;

	i = 0;
	tmp = args;
	while (tmp)
	{
		if (tmp->content)
			i++;
		tmp = tmp->next;
	}
	return (i);
}

static int	is_numeric(char *str)
{
	int	j;

	j = 0;
	while (str[j])
	{
		if (str[0] == '-' || str[0] == '+')
			j++;
		if (ft_isdigit(str[j]) == 0)
			return (0);
		j++;
	}
	return (1);
}

static void	exit_numeric_error(t_exit_ctx *ctx, char *str)
{
	char	*error;

	if (ctx->in_child != 1)
		write(1, "exit\n", 6);
	error = ft_strjoin_modif("minishell: exit: ", str);
	error = ft_strjoin_modif(error, ": numeric argument required\n");
	write(2, error, ft_strlen(error));
	exit_cleanup(ctx);
	exit(2);
}

static int	exit_too_many_args(t_exit_ctx *ctx)
{
	if (ctx->in_child != 1)
		write(1, "exit\n", 6);
	write(2, "minishell: exit: too many arguments\n", 37);
	change_exit_value("1", ctx->envp);
	return (1);
}

int	builtin_exit(t_exit_ctx *ctx)
{
	char	*str;
	int		i;
	int		num;

	if (!ctx->args)
	{
		exit_cleanup(ctx);
		exit(0);
	}
	i = count_args(ctx->args);
	str = ctx->args->content;
	if (!is_numeric(str))
		exit_numeric_error(ctx, str);
	if (i > 1)
		return (exit_too_many_args(ctx));
	num = ft_atoi(str);
	if (num < 0 || num > 255)
		num = num % 256;
	if (ctx->in_child != 1)
		write(1, "exit\n", 6);
	exit_cleanup(ctx);
	exit(num);
}

void	change_exit_value(char *value, char ***envp)
{
	int		idx;
	char	*entry;

	if (!value)
		value = "0";
	entry = ft_strjoin("return_value=", value);
	if (!entry)
		return ;
	idx = find_env_index(*envp, "return_value");
	if (idx >= 0)
		replace_env_value(*envp, idx, entry);
	else
		append_env_value(envp, entry);
	free(entry);
}

```

### `execution/close_fds.c`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   close_fds.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 17:56:01 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/21 05:38:32 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	close_fds(int **arr_fds, int len_cmds)
{
	int	j;

	j = 0;
	if (arr_fds == NULL)
		return ;
	while (j < len_cmds - 1 && arr_fds != NULL)
	{
		close(arr_fds[j][0]);
		close(arr_fds[j][1]);
		j++;
	}
}

```

### `execution/execution.c`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 07:47:27 by oscar             #+#    #+#             */
/*   Updated: 2026/03/21 11:22:27 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
#include "../built_ins/provisionaryFunctions.h"

static void	error_fork(t_exec_pack *p)
{
	perror("minishell: fork");
	change_exit_value("1", &p->envp);
	ft_free_malloc();
	free_envp_copy(p->envp);
	cmd_list_destroy(*p->data);
	free(p->line);
	intvec_destroy(p->codes);
	exit(1);
}

static void	error_pipe(t_exec_pack *p)
{
	perror("minishell: pipe");
	change_exit_value("1", &p->envp);
	free_envp_copy(p->envp);
	cmd_list_destroy(*p->data);
	free(p->line);
	intvec_destroy(p->codes);
	exit(1);
}

static void	error_fd_file(char *name_infile, t_exec_pack *p)
{
	char	*error;

	error = ft_strjoin_modif("minishell: ", name_infile);
	perror(error);
	ft_free_malloc();
	change_exit_value("1", &p->envp);
	free_envp_copy(p->envp);
	cmd_list_destroy(*p->data);
	free(p->line);
	intvec_destroy(p->codes);
	exit(1);
}

char	**get_args(t_list *phrase)
{
	t_list	*tmp;
	int		args_count;
	char	**args;
	int		i;

	args_count = 0;
	i = 0;
	tmp = phrase;
	while (tmp != NULL)
	{
		args_count++;
		tmp = tmp->next;
	}
	args = (char **)ft_malloc(sizeof(char *) * (args_count + 2));
	tmp = phrase;
	while (i < args_count)
	{
		args[i + 1] = tmp->content;
		tmp = tmp->next;
		i++;
	}
	args[args_count + 1] = NULL;
	return (args);
}

static void	exit_error(t_exec_pack *p, char *code, int exit_code)
{
	change_exit_value(code, &p->envp);
	free_envp_copy(p->envp);
	cmd_list_destroy(*p->data);
	free(p->line);
	intvec_destroy(p->codes);
	exit(exit_code);
}

static void	handle_fok_error(t_exec_pack *p, char *data_cmd)
{
	char	*error;

	error = ft_strjoin_modif("minishell: ", data_cmd);
	perror(error);
	close_fds(p->arr_fds, p->len);
	ft_free_malloc();
	if (errno == ENOTDIR)
		exit_error(p, "126", 126);
	else if (errno == ENOENT)
		exit_error(p, "127", 127);
}

static void	handle_xok_error(t_exec_pack *p, char *data_cmd)
{
	char	*error;

	error = ft_strjoin_modif("minishell: ", data_cmd);
	perror(error);
	change_exit_value("126", &p->envp);
	close_fds(p->arr_fds, p->len);
	free_envp_copy(p->envp);
	cmd_list_destroy(*p->data);
	free(p->line);
	intvec_destroy(p->codes);
	exit(126);
}

static char	*handle_direct_cmd(t_exec_pack *p, char *data_cmd)
{
	if (access(data_cmd, F_OK) != 0)
		handle_fok_error(p, data_cmd);
	if (access(data_cmd, X_OK) != 0)
		handle_xok_error(p, data_cmd);
	return (data_cmd);
}

static char	*get_cmd_name_and_path(t_exec_pack *p,
	char **args, char *data_cmd)
{
	char	*cmd_name;
	char	*cmd_path;

	cmd_path = NULL;
	if (find_chr(data_cmd, '/') == 1)
	{
		cmd_name = strchr_modif(data_cmd, '/');
		cmd_path = handle_direct_cmd(p, data_cmd);
		args[0] = cmd_name;
	}
	else
	{
		cmd_path = get_cmd_path(p, data_cmd);
		args[0] = data_cmd;
	}
	return (cmd_path);
}


static void	handle_infile_redirections(t_list_red **tmp_infiles,
	t_exec_pack *p)
{
	int				fd_file;
	t_heredoc_fork	ctx;
	char			*content;
	int				pipefd[2];

	while (*tmp_infiles != NULL)
	{
		if ((*tmp_infiles)->doble == 1)
		{
			ctx.delimiter = (*tmp_infiles)->content;
			ctx.expand = 1;
			ctx.env = p->envp;
			ctx.data = p->data;
			ctx.line = p->line;
			ctx.codes = p->codes;
			content = heredoc_with_fork(&ctx);
			if (!content)
			{
				change_exit_value("130", &p->envp);
				ft_free_malloc();
				free_envp_copy(p->envp);
				cmd_list_destroy(*p->data);
				free(p->line);
				intvec_destroy(p->codes);
				exit(130);
			}
			pipe(pipefd);
			write(pipefd[1], content, ft_strlen(content));
			close(pipefd[1]);
			dup2(pipefd[0], STDIN_FILENO);
			close(pipefd[0]);
			free(content);
		}
		else
		{
			fd_file = open((*tmp_infiles)->content, O_RDONLY);
			if (fd_file == -1)
			{
				close_fds(p->arr_fds, p->len);
				error_fd_file((*tmp_infiles)->content, p);
			}
			dup2(fd_file, STDIN_FILENO);
			close(fd_file);
		}
		*tmp_infiles = (*tmp_infiles)->next;
	}
}

static void	handle_outfile_redirections(t_list_red **tmp_outfiles,
	t_exec_pack *p)
{
	int	fd_file;

	while (*tmp_outfiles != NULL)
	{
		if ((*tmp_outfiles)->doble == 0)
			fd_file = open((*tmp_outfiles)->content,
					O_WRONLY | O_CREAT | O_TRUNC, 0666);
		else
			fd_file = open((*tmp_outfiles)->content,
					O_WRONLY | O_CREAT | O_APPEND, 0666);
		if (fd_file == -1)
		{
			close_fds(p->arr_fds, p->len);
			error_fd_file((*tmp_outfiles)->content, p);
		}
		dup2(fd_file, STDOUT_FILENO);
		close(fd_file);
		*tmp_outfiles = (*tmp_outfiles)->next;
	}
}

int	execute_cmds(t_cmdnode **data, char ***envp, char *line, t_intvec *codes)
{
	pid_t		*pids;
	char		**args;
	char		*cmd_path;
	int			len;
	int			i;
	int			last_exit_status;
	int			**arr_fds;
	t_cmdnode	*data_tmp;
	int			saved_stdin;
	int			saved_stdout;
	t_list_red	*tmp_infiles;
	int			fd_file;
	char		*error;
	t_list_red	*tmp_outfiles;
	int			return_value;
	t_exit_ctx	ctx;
	int			signal_code;
	char		*status_str;
	t_exec_pack	p;

	len = 0;
	i = 0;
	last_exit_status = 0;
	data_tmp = *data;
	pids = NULL;
	arr_fds = NULL;
	while (data_tmp != NULL)
	{
		len++;
		data_tmp = data_tmp->next;
	}
	if (len > 1)
	{
		arr_fds = (int **)ft_malloc(sizeof(int *) * (len - 1));
		while (i < len - 1)
		{
			arr_fds[i] = (int *)ft_malloc(sizeof(int) * 2);
			if (pipe(arr_fds[i]) == -1)
			{
				p.envp = *envp;
				p.data = data;
				p.line = line;
				p.codes = codes;
				p.arr_fds = arr_fds;
				p.len = len;
				error_pipe(&p);
			}
			i++;
		}
	}
	p.envp = *envp;
	p.data = data;
	p.line = line;
	p.codes = codes;
	p.arr_fds = arr_fds;
	p.len = len;

	i = 0;
	data_tmp = *data;

	if (len == 1 && (data_tmp->cmd == NULL || data_tmp->cmd[0] == '\0'))
	{
		saved_stdin = -1;
		saved_stdout = -1;
		if (data_tmp->infiles != NULL)
		{
			tmp_infiles = data_tmp->infiles;
			saved_stdin = dup(STDIN_FILENO);
			while (tmp_infiles != NULL)
			{
				if (tmp_infiles->doble == 1)
					tmp_infiles = tmp_infiles->next;
				else
				{
					fd_file = open(tmp_infiles->content, O_RDONLY);
					if (fd_file == -1)
					{
						error = ft_strjoin_modif("minishell: ", (char *)tmp_infiles->content);
						perror(error);
						change_exit_value("1", envp);
						return (1);
					}
					dup2(fd_file, STDIN_FILENO);
					close(fd_file);
					tmp_infiles = tmp_infiles->next;
				}
			}
		}
		if (data_tmp->outfiles != NULL)
		{
			tmp_outfiles = data_tmp->outfiles;
			saved_stdout = dup(STDOUT_FILENO);
			while (tmp_outfiles != NULL)
			{
				if (tmp_outfiles->doble == 0)
					fd_file = open(tmp_outfiles->content, O_WRONLY | O_CREAT | O_TRUNC, 0666);
				if (tmp_outfiles->doble == 1)
					fd_file = open(tmp_outfiles->content, O_WRONLY | O_CREAT | O_APPEND, 0666);
				if (fd_file == -1)
				{
					error = ft_strjoin_modif("minishell: ", (char *)tmp_outfiles->content);
					perror(error);
					change_exit_value("1", envp);
					return (1);
				}
				dup2(fd_file, STDOUT_FILENO);
				close(fd_file);
				tmp_outfiles = tmp_outfiles->next;
			}
		}
		if (saved_stdin != -1)
		{
			dup2(saved_stdin, STDIN_FILENO);
			close(saved_stdin);
		}
		if (saved_stdout != -1)
		{
			dup2(saved_stdout, STDOUT_FILENO);
			close(saved_stdout);
		}
		change_exit_value("0", envp);
		return (0);
	}

	if (len == 1 && (is_built_in(data_tmp->cmd) == 1))
	{
		saved_stdin = -1;
		tmp_infiles = data_tmp->infiles;
		if (tmp_infiles != NULL)
		{
			saved_stdin = dup(STDIN_FILENO);
			while (tmp_infiles != NULL)
			{
				fd_file = open(tmp_infiles->content, O_RDONLY);
				if (fd_file == -1)
				{
					close_fds(arr_fds, len);
					error_fd_file(tmp_infiles->content, &p);
				}
				dup2(fd_file, STDIN_FILENO);
				close(fd_file);
				tmp_infiles = tmp_infiles->next;
			}
		}
		saved_stdout = -1;
		tmp_outfiles = data_tmp->outfiles;
		if (tmp_outfiles != NULL)
		{
			saved_stdout = dup(STDOUT_FILENO);
			while (tmp_outfiles != NULL)
			{
				if (tmp_outfiles->doble == 0)
					fd_file = open(tmp_outfiles->content, O_WRONLY | O_CREAT | O_TRUNC, 0666);
				if (tmp_outfiles->doble == 1)
					fd_file = open(tmp_outfiles->content, O_WRONLY | O_CREAT | O_APPEND, 0666);
				if (fd_file == -1)
				{
					close_fds(arr_fds, len);
					error_fd_file(tmp_outfiles->content, &p);
				}
				dup2(fd_file, STDOUT_FILENO);
				close(fd_file);
				tmp_outfiles = tmp_outfiles->next;
			}
		}

		return_value = 0;

		if (data_tmp && (ft_strncmp(data_tmp->cmd, "echo", 4) == 0 && data_tmp->cmd[4] == '\0'))
			return_value = builtin_echo(data_tmp->phrase);
		else if (data_tmp && (ft_strncmp(data_tmp->cmd, "cd", 2) == 0 && data_tmp->cmd[2] == '\0'))
			return_value = builtin_cd(data_tmp->phrase, *envp);
		else if (data_tmp && (ft_strncmp(data_tmp->cmd, "pwd", 3) == 0 && data_tmp->cmd[3] == '\0'))
			return_value = builtin_pwd();
		else if (data_tmp && (ft_strncmp(data_tmp->cmd, "export", 6) == 0 && data_tmp->cmd[6] == '\0'))
			return_value = builtin_export(data_tmp->phrase, envp);
		else if (data_tmp && (ft_strncmp(data_tmp->cmd, "unset", 5) == 0 && data_tmp->cmd[5] == '\0'))
			return_value = builtin_unset(data_tmp->phrase, envp);
		else if (data_tmp && (ft_strncmp(data_tmp->cmd, "env", 3) == 0 && data_tmp->cmd[3] == '\0'))
			return_value = builtin_env(*envp);
		else if (data_tmp && (ft_strncmp(data_tmp->cmd, "exit", 4) == 0 && data_tmp->cmd[4] == '\0'))
		{
			ctx.args = data_tmp->phrase;
			ctx.in_child = 0;
			ctx.envp = envp;
			ctx.data = data;
			ctx.line = line;
			ctx.codes = codes;
			return_value = builtin_exit(&ctx);
		}

		if (saved_stdin != -1)
		{
			dup2(saved_stdin, STDIN_FILENO);
			close(saved_stdin);
		}
		if (saved_stdout != -1)
		{
			dup2(saved_stdout, STDOUT_FILENO);
			close(saved_stdout);
		}
		return (return_value);
	}

	else
	{
		pids = ft_malloc(sizeof(pid_t) * len);
		signal(SIGINT, handle_sigint);
		signal(SIGQUIT, SIG_IGN);
		while (i < len)
		{
			pids[i] = fork();
			if (pids[i] == -1)
				error_fork(&p);
			else if (pids[i] == 0)
			{
				signal(SIGINT, SIG_DFL);
				signal(SIGQUIT, SIG_DFL);

				if (i == 0)
				{
					tmp_infiles = data_tmp->infiles;
					if (tmp_infiles != NULL)
						handle_infile_redirections(&tmp_infiles, &p);
					tmp_outfiles = data_tmp->outfiles;
					if (tmp_outfiles != NULL)
						handle_outfile_redirections(&tmp_outfiles, &p);
					if (len > 1 && data_tmp->outfiles == NULL)
						dup2(arr_fds[0][1], STDOUT_FILENO);
				}
				else if (i == len - 1)
				{
					tmp_infiles = data_tmp->infiles;
					if (tmp_infiles != NULL)
						handle_infile_redirections(&tmp_infiles, &p);
					else
						dup2(arr_fds[i - 1][0], STDIN_FILENO);
					tmp_outfiles = data_tmp->outfiles;
					if (tmp_outfiles != NULL)
						handle_outfile_redirections(&tmp_outfiles, &p);
				}
				else
				{
					tmp_infiles = data_tmp->infiles;
					if (tmp_infiles != NULL)
						handle_infile_redirections(&tmp_infiles, &p);
					else
						dup2(arr_fds[i - 1][0], STDIN_FILENO);

					tmp_outfiles = data_tmp->outfiles;
					if (tmp_outfiles != NULL)
						handle_outfile_redirections(&tmp_outfiles, &p);
					else
						dup2(arr_fds[i][1], STDOUT_FILENO);
				}

				close_fds(arr_fds, len);

				return_value = 0;

				if (data_tmp && (ft_strncmp(data_tmp->cmd, "echo", 4) == 0 && data_tmp->cmd[4] == '\0'))
					return_value = builtin_echo(data_tmp->phrase);
				else if (data_tmp && (ft_strncmp(data_tmp->cmd, "cd", 2) == 0 && data_tmp->cmd[2] == '\0'))
					return_value = builtin_cd(data_tmp->phrase, *envp);
				else if (data_tmp && (ft_strncmp(data_tmp->cmd, "pwd", 3) == 0 && data_tmp->cmd[3] == '\0'))
					return_value = builtin_pwd();
				else if (data_tmp && (ft_strncmp(data_tmp->cmd, "export", 6) == 0 && data_tmp->cmd[6] == '\0'))
					return_value = builtin_export(data_tmp->phrase, envp);
				else if (data_tmp && (ft_strncmp(data_tmp->cmd, "unset", 5) == 0 && data_tmp->cmd[5] == '\0'))
					return_value = builtin_unset(data_tmp->phrase, envp);
				else if (data_tmp && (ft_strncmp(data_tmp->cmd, "env", 3) == 0 && data_tmp->cmd[3] == '\0'))
					return_value = builtin_env(*envp);
				else if (data_tmp && (ft_strncmp(data_tmp->cmd, "exit", 4) == 0 && data_tmp->cmd[4] == '\0'))
				{
					ctx.args = data_tmp->phrase;
					ctx.in_child = 1;
					ctx.envp = envp;
					ctx.data = data;
					ctx.line = line;
					ctx.codes = codes;
					return_value = builtin_exit(&ctx);
				}
				else
				{
					if (data_tmp->cmd != NULL)
						args = get_args(data_tmp->phrase);
					else
						args = NULL;

					if (data_tmp->cmd != NULL)
						cmd_path = get_cmd_name_and_path(&p, args, data_tmp->cmd);
					else
						cmd_path = NULL;

					if (cmd_path && args)
					{
						if (execve(cmd_path, args, *envp) < 0)
						{
							error = ft_strjoin_modif("minishell: ", cmd_path);
							perror(error);
							if (errno == EACCES)
							{
								change_exit_value("126", envp);
								exit(126);
							}
							else if (errno == ENOENT)
							{
								change_exit_value("127", envp);
								exit(127);
							}
							else
							{
								change_exit_value("126", envp);
								exit(126);
							}
						}
					}
				}

				ft_free_malloc();
				free_envp_copy(*envp);
				cmd_list_destroy(*data);
				free(line);
				intvec_destroy(codes);
				exit(return_value);
			}
			i++;
			data_tmp = data_tmp->next;
		}

		close_fds(arr_fds, len);

		i = 0;
		while (i < len)
		{
			waitpid(pids[i], &last_exit_status, 0);
			i++;
		}

		signal(SIGINT, handle_sigint);
		signal(SIGQUIT, SIG_IGN);
	}

	signal_code = 0;

	if (WIFEXITED(last_exit_status))
		last_exit_status = WEXITSTATUS(last_exit_status);
	else if (WIFSIGNALED(last_exit_status))
	{
		signal_code = WTERMSIG(last_exit_status);
		if (signal_code == SIGINT)
			write(1, "", 0);
		else if (signal_code == SIGQUIT)
			write(1, "Quit (core dumped)\n", 20);
		last_exit_status = signal_code + 128;
	}

	status_str = ft_itoa(last_exit_status);
	change_exit_value(status_str, envp);
	return (last_exit_status);
}

```

### `execution/find_chr.c`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   find_chr.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 12:13:32 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/13 14:02:40 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	find_chr(const char *s, int c)
{
	unsigned char	ch;
	int				i;

	ch = (unsigned char)c;
	i = 0;
	while (s[i] != '\0')
	{
		if (s[i] == ch)
			return (1);
		i++;
	}
	if (ch == '\0')
		return (0);
	return (0);
}

```

### `execution/get_cmd_path.c`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_cmd_path.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 23:02:00 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/21 07:50:45 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	**get_all_paths(char **envp)
{
	char	**paths;
	char	*full_path;
	int		i;

	i = 0;
	paths = NULL;
	full_path = NULL;
	while (envp && envp[i] != NULL)
	{
		if (ft_strncmp(envp[i], "PATH=", 5) == 0)
		{
			full_path = envp[i] + 5;
			break ;
		}
		i++;
	}
	paths = ft_split(full_path, ':');
	return (paths);
}

static char	*find_path(char **paths, char *cmd_name)
{
	char	*cmd_path;
	char	*temp;
	int		i;

	i = 0;
	cmd_path = NULL;
	temp = NULL;
	while (paths != NULL && paths[i] != NULL)
	{
		temp = ft_strjoin_modif(paths[i], "/");
		cmd_path = ft_strjoin_modif(temp, cmd_name);
		if (access(cmd_path, X_OK) == 0)
			return (cmd_path);
		i++;
	}
	return (NULL);
}

char	*get_cmd_path(t_exec_pack *p, char *cmd_name)
{
	char	**paths;
	char	*cmd_path;
	char	*error;

	paths = get_all_paths(p->envp);
	cmd_path = find_path(paths, cmd_name);
	if (cmd_path == NULL)
	{
		if (find_chr(cmd_name, '/') == 1)
			error = ft_strjoin_modif("minishell: ", cmd_name);
		else
			error = cmd_name;
		error = ft_strjoin_modif(error, ": command not found\n");
		write(2, error, ft_strlen(error));
		close_fds(p->arr_fds, p->len);
		ft_free_malloc();
		free_envp_copy(p->envp);
		cmd_list_destroy(*p->data);
		free(p->line);
		intvec_destroy(p->codes);
		exit(127);
	}
	return (cmd_path);
}

```

### `execution/is_built_in.c`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   is_built_in.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/03 18:56:12 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/16 15:42:50 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	is_built_in(char *cmd_name)
{
	int	built_in;

	built_in = 0;
	if (ft_strncmp (cmd_name, "echo", 4) == 0 && cmd_name[4] == '\0')
		built_in = 1;
	else if (ft_strncmp (cmd_name, "cd", 2) == 0 && cmd_name[2] == '\0')
		built_in = 1;
	else if (ft_strncmp (cmd_name, "pwd", 3) == 0 && cmd_name[3] == '\0')
		built_in = 1;
	else if (ft_strncmp (cmd_name, "export", 6) == 0 && cmd_name[6] == '\0')
		built_in = 1;
	else if (ft_strncmp (cmd_name, "unset", 5) == 0 && cmd_name[5] == '\0')
		built_in = 1;
	else if (ft_strncmp (cmd_name, "env", 3) == 0 && cmd_name[3] == '\0')
		built_in = 1;
	else if (ft_strncmp (cmd_name, "exit", 4) == 0 && cmd_name[4] == '\0')
		built_in = 1;
	return (built_in);
}

```

### `execution/strchr_modif.c`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strchr_modif.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 12:13:32 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/17 17:12:18 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*strchr_modif(const char *s, int c)
{
	unsigned char	ch;
	int				i;
	int				len;

	ch = (unsigned char)c;
	len = ft_strlen(s);
	i = len - 1;
	while (i > 0)
	{
		if (s[i] == ch)
			return ((char *)&s[i + 1]);
		i--;
	}
	if (ch == '\0')
		return ((char *)&s[i]);
	return (NULL);
}

```

### `ft_utils/ft_atoi.c`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 14:00:25 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/19 13:43:19 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	ft_atoi(const char *nptr)
{
	int	i;
	int	sign;
	int	num;

	i = 0;
	sign = 1;
	num = 0;
	while ((nptr[i] == 32) || (nptr[i] >= 9 && nptr[i] <= 13))
		i++;
	if ((nptr[i] == '+') || (nptr[i] == '-'))
	{
		if (nptr[i] == '-')
			sign = -1;
		i++;
	}
	while ((nptr[i] >= '0') && (nptr[i] <= '9'))
	{
		num = num * 10;
		num = num + nptr[i] - '0';
		i++;
	}
	return (sign * num);
}

```

### `ft_utils/ft_isdigit.c`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isdigit.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 14:45:20 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/19 13:40:17 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	ft_isdigit(int c)
{
	if (c >= 48 && c <= 57)
		return (2048);
	else
		return (0);
}

```

### `ft_utils/ft_itoa.c`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 16:01:35 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/17 16:00:54 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	countdigits_sign(int n, int *sign, long *num)
{
	int	counter;

	counter = 0;
	if (n < 0)
	{
		*sign = 1;
		*num = -*num;
	}
	while (n != 0)
	{
		n = n / 10;
		counter++;
	}
	return (counter);
}

static void	fill_ptr(char *ptr, long num, int counter)
{
	while (num != 0)
	{
		counter--;
		ptr[counter] = (num % 10) + '0';
		num = num / 10;
	}
}

char	*ft_itoa(int n)
{
	char	*ptr;
	int		sign;
	int		counter;
	long	num;

	num = n;
	sign = 0;
	if (n == 0)
	{
		ptr = (char *)ft_malloc(2);
		if (ptr == NULL)
			return (NULL);
		return (ptr[0] = '0', ptr[1] = '\0', ptr);
	}
	counter = countdigits_sign(n, &sign, &num);
	ptr = (char *)ft_malloc(sign + counter + 1);
	if (sign == 1)
		counter++;
	ptr[counter] = '\0';
	fill_ptr(ptr, num, counter);
	if (sign == 1)
		ptr[0] = '-';
	return (ptr);
}

```

### `ft_utils/ft_lstadd_back.c`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_back.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/03 19:10:02 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/17 13:02:26 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	ft_lstadd_back(t_list **lst, t_list *new)
{
	t_list	*temp;

	new->next = NULL;
	if (*lst == NULL)
		*lst = new;
	else
	{
		temp = *lst;
		while (temp->next != NULL)
			temp = temp->next;
		temp->next = new;
		new->prev = temp;
	}
}

```

### `ft_utils/ft_lstnew.c`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstnew.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/03 19:07:30 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/13 11:46:04 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_list	*ft_lstnew(void *content)
{
	t_list	*lst;

	lst = malloc(sizeof(t_list));
	if (lst == NULL)
		return (NULL);
	lst->content = content;
	lst->prev = NULL;
	lst->next = NULL;
	return (lst);
}

```

### `ft_utils/ft_mallocs_list.c`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_mallocs_list.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/03 18:00:32 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/17 12:58:37 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	ft_free(t_list *mallocs_list)
{
	t_list	*temp;

	while (mallocs_list != NULL)
	{
		temp = mallocs_list->next;
		free(mallocs_list->content);
		free(mallocs_list);
		mallocs_list = temp;
	}
}

static void	*register_malloc(void *ptr, t_list **lst)
{
	t_list	*node;

	node = ft_lstnew(ptr);
	if (!node)
	{
		free(ptr);
		ft_free(*lst);
		exit(1);
	}
	ft_lstadd_back(lst, node);
	return (ptr);
}

static void	*ft_malloc_core(size_t size, int flag)
{
	static t_list	*mallocs_list;
	void			*result;

	result = NULL;
	if (flag == 1)
	{
		result = malloc(size);
		if (!result)
		{
			ft_free(mallocs_list);
			exit(1);
		}
		result = register_malloc(result, &mallocs_list);
	}
	else if (flag == 2)
		ft_free(mallocs_list);
	return (result);
}

void	*ft_malloc(size_t size)
{
	return (ft_malloc_core(size, 1));
}

void	ft_free_malloc(void)
{
	ft_malloc_core(0, 2);
}

```

### `ft_utils/ft_memcpy.c`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 14:53:05 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/16 00:19:22 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	unsigned char		*u_dest;
	const unsigned char	*u_src;

	u_dest = (unsigned char *)dest;
	u_src = (const unsigned char *)src;
	while (n > 0)
	{
		*u_dest = *u_src;
		u_dest++;
		u_src++;
		n--;
	}
	return (dest);
}

```

### `ft_utils/ft_memset.c`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 17:20:33 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/16 00:10:57 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	*ft_memset(void *s, int c, size_t n)
{
	unsigned char	*ch;
	size_t			i;

	ch = (unsigned char *)s;
	i = 0;
	while (n > 0)
	{
		ch[i] = c;
		i++;
		n--;
	}
	return (ch);
}

```

### `ft_utils/ft_realloc.c`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_realloc.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/17 12:35:14 by oscar             #+#    #+#             */
/*   Updated: 2026/03/17 13:00:46 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	*ft_realloc(void *ptr, size_t old_size, size_t new_size)
{
	size_t			i;
	unsigned char	*new_ptr;
	unsigned char	*old_ptr;

	if (new_size == 0)
	{
		free(ptr);
		return (NULL);
	}
	new_ptr = (unsigned char *)malloc(new_size);
	if (!new_ptr)
		return (NULL);
	if (!ptr)
		return (new_ptr);
	old_ptr = (unsigned char *)ptr;
	i = 0;
	while (i < old_size && i < new_size)
	{
		new_ptr[i] = old_ptr[i];
		i++;
	}
	free(ptr);
	return (new_ptr);
}

```

### `ft_utils/ft_split.c`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 16:18:45 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/17 16:07:59 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static size_t	count_words(char const *s, char c)
{
	int		i;
	int		words;
	size_t	len_s;

	i = 0;
	words = 0;
	len_s = ft_strlen(s);
	while (s[i] != '\0')
	{
		if (s[i] == c && s[i + 1] != c)
			words++;
		i++;
	}
	if (s[len_s - 1] == c && len_s != 0)
		words--;
	if (s[0] == c && c != '\0')
		words--;
	if (s[0] != '\0')
		words++;
	return (words);
}

static void	find_start_end(char const **s, char c, char **start, char **end)
{
	*start = *end;
	while ((**s != '\0') && (*start == *end))
	{
		if (**s == c)
			(*s)++;
		if (**s != c)
			*start = (char *)*s;
		(*s)++;
	}
	*end = *start;
	while (*end == *start)
	{
		if ((**s == c) || (**s == '\0'))
			*end = (char *)*s;
		(*s)++;
	}
}

char	**ft_split(char const *s, char c)
{
	char	**arr;
	int		j;
	int		words;
	char	*start;
	char	*end;

	if (s == NULL)
		return (NULL);
	j = -1;
	start = NULL;
	end = NULL;
	words = count_words(s, c);
	arr = (char **)ft_malloc(sizeof(char *) * (words + 1));
	arr[words] = NULL;
	while (++j < words)
	{
		find_start_end(&s, c, &start, &end);
		arr[j] = (char *)ft_malloc(end - start + 1);
		ft_strlcpy(&arr[j][0], start, (int)(end - start + 1));
	}
	return ((char **)arr);
}

```

### `ft_utils/ft_strchr.c`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 12:13:32 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/17 13:10:37 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*ft_strchr(const char *s, int c)
{
	unsigned char	ch;
	int				i;

	ch = (unsigned char)c;
	i = 0;
	while (s[i] != '\0')
	{
		if (s[i] == ch)
			return ((char *)&s[i]);
		i++;
	}
	if (ch == '\0')
		return ((char *)&s[i]);
	return (NULL);
}

```

### `ft_utils/ft_strcmp.c`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/17 12:34:36 by oscar             #+#    #+#             */
/*   Updated: 2026/03/17 12:56:48 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	ft_strcmp(const char *s1, const char *s2)
{
	size_t	i;

	if (!s1 || !s2)
		return (0);
	i = 0;
	while (s1[i] && s2[i] && s1[i] == s2[i])
		i++;
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}

```

### `ft_utils/ft_strdup.c`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 12:32:00 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/20 13:42:14 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*ft_strdup(const char *s)
{
	unsigned char	*ch;
	size_t			size;
	size_t			i;
	unsigned char	*ptr;

	size = 0;
	i = 0;
	ch = (unsigned char *)s;
	while (ch[i] != '\0')
		i++;
	size = i + 1;
	ptr = malloc(size);
	if (ptr == NULL)
		return (NULL);
	i = 0;
	while (i < size)
	{
		ptr[i] = ch[i];
		i++;
	}
	return ((char *)ptr);
}

```

### `ft_utils/ft_strjoin.c`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 15:07:38 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/17 16:11:19 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*ptr;
	int		i;
	int		z;
	int		len_s1;
	int		len_s2;

	i = -1;
	z = 0;
	if (s1 == NULL || s2 == NULL)
		return (NULL);
	len_s1 = ft_strlen(s1);
	len_s2 = ft_strlen(s2);
	ptr = malloc((len_s1 + len_s2 + 1) * sizeof(char));
	if (ptr == NULL)
		return (NULL);
	while (++i < len_s1)
		ptr[i] = s1[i];
	z = i;
	i = 0;
	ft_strlcpy(&ptr[z], s2, len_s2 + 1);
	return (ptr);
}

```

### `ft_utils/ft_strjoin_modif.c`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin_modif.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 15:07:38 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/17 16:10:25 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*ft_strjoin_modif(char const *s1, char const *s2)
{
	char	*ptr;
	int		i;
	int		z;
	int		len_s1;
	int		len_s2;

	i = -1;
	z = 0;
	if (s1 == NULL || s2 == NULL)
		return (NULL);
	len_s1 = ft_strlen(s1);
	len_s2 = ft_strlen(s2);
	ptr = ft_malloc((len_s1 + len_s2 + 1) * sizeof(char));
	while (++i < len_s1)
		ptr[i] = s1[i];
	z = i;
	i = 0;
	ft_strlcpy(&ptr[z], s2, len_s2 + 1);
	return (ptr);
}

```

### `ft_utils/ft_strlcpy.c`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 15:23:08 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/13 11:46:26 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

size_t	ft_strlcpy(char *dst, const char *src, size_t size)
{
	size_t	i;
	size_t	len_src;

	i = 0;
	len_src = 0;
	while (src[i] != '\0')
		i++;
	len_src = i;
	if (size == 0)
		return (len_src);
	i = 0;
	while (i < size - 1 && (src[i] != '\0'))
	{
		dst[i] = src[i];
		i++;
	}
	dst[i] = '\0';
	i = 0;
	return (len_src);
}

```

### `ft_utils/ft_strlen.c`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlen.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 15:19:18 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/21 04:53:36 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

size_t	ft_strlen(const char *s)
{
	size_t	num;

	num = 0;
	if (!s)
	{
		printf("ERROR: ft_strlen NULL\n");
		return (0);
	}
	while (*s != '\0')
	{
		num++;
		s++;
	}
	return (num);
}

```

### `ft_utils/ft_strncmp.c`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 14:41:33 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/13 11:46:36 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	unsigned char	*us1;
	unsigned char	*us2;
	size_t			i;

	i = 0;
	us1 = (unsigned char *)s1;
	us2 = (unsigned char *)s2;
	while (n > 0 && us1[i] != '\0' && us2[i] != '\0')
	{
		if (us1[i] == us2[i])
			i++;
		else
			return (us1[i] - us2[i]);
		n--;
	}
	if (n > 0)
		return (us1[i] - us2[i]);
	return (0);
}

```

### `ft_utils/ft_strndup.c`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strndup.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/17 12:33:39 by oscar             #+#    #+#             */
/*   Updated: 2026/03/17 13:01:31 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*ft_strndup(const char *s, size_t n)
{
	size_t	i;
	char	*res;

	if (!s)
		return (NULL);
	i = 0;
	while (s[i] && i < n)
		i++;
	res = (char *)malloc(i + 1);
	if (!res)
		return (NULL);
	i = 0;
	while (s[i] && i < n)
	{
		res[i] = s[i];
		i++;
	}
	res[i] = '\0';
	return (res);
}

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
/*   Updated: 2026/03/21 05:34:49 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
# define PARSER_H

# include "../minishell.h"
# include "../built_ins/provisionaryFunctions.h"

/* list utils */
void		intvec_push(t_intvec *v, int x);
void		intvec_free(t_intvec *v);
void		str_list_push_back(t_list **head, const char *s);
void		str_list_free(t_list *lst);
t_list_red	*red_list_push_back(t_list_red **head, const char *s, int doble);
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
int			store_pending_redir(t_parse_ctx *ctx, const char *target);

/* operator handler */
int			handle_operator(t_parse_ctx *ctx);

/* command */
int			handle_command(const char *line, t_parser *p,
				t_intvec *codes, char **env);

/* parser entry */
int			parse_line(const char *line, t_intvec *codes,
				t_cmdnode **out_list, char **env);

void		parser_init(t_parser *p, t_cmdnode **out_list);
int			parser_error(t_parser *p, t_intvec *codes);
void		finalize_pending(t_parser *p);
int			parse_loop(t_parse_ctx *ctx);
char		*get_clean_token(const char *line, size_t *i, char **env);

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
/*   Updated: 2026/03/19 23:13:15 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

char	*get_clean_token(const char *line, size_t *i, char **env)
{
	char	*tok;
	char	*expanded;
	char	*clean;
	int		grouped;

	grouped = 0;
	tok = read_token_grouped(line, i, &grouped);
	if (!tok)
		return (NULL);
	expanded = expand_env_vars(tok, env);
	free(tok);
	if (!expanded)
		return (NULL);
	clean = remove_quotes_str(expanded);
	free(expanded);
	return (clean);
}

static int	add_command_token(const char *line, t_parser *p, char **env)
{
	char	*clean;

	clean = get_clean_token(line, &p->i, env);
	if (!clean)
		return (1);
	if (clean[0] == '\0')
	{
		free(clean);
		return (0);
	}
	if (!p->current_cmd->cmd)
		p->current_cmd->cmd = ft_strdup(clean);
	else
		str_list_push_back(&p->current_cmd->phrase, clean);
	free(clean);
	return (0);
}

int	handle_command(const char *line, t_parser *p,
				t_intvec *codes, char **env)
{
	if (codes)
	{
	}
	if (!p->current_cmd)
	{
		p->current_cmd = node_new(NULL);
		cmd_list_append(&p->head, &p->tail, p->current_cmd);
	}
	if (add_command_token(line, p, env))
		return (1);
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
/*   Updated: 2026/03/21 05:26:04 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

void	parser_init(t_parser *p, t_cmdnode **out_list)
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
	p->last_pipe = 1;
	*out_list = NULL;
}

int	parser_error(t_parser *p, t_intvec *codes)
{
	cmd_list_free(p->head);
	red_list_free(p->pending_in);
	red_list_free(p->pending_out);
	intvec_free(codes);
	return (-1);
}

void	finalize_pending(t_parser *p)
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

int	parse_loop(t_parse_ctx *ctx)
{
	while (ctx->line[ctx->p->i])
	{
		skip_spaces(ctx->line, &ctx->p->i);
		if (!ctx->line[ctx->p->i])
			break ;
		ctx->p->op_match = match_op(ctx->line,
				ctx->p->i, &ctx->p->op_len, &ctx->p->op_code);
		if (ctx->p->op_match == -1)
			return (-1);
		if (ctx->p->op_match == 1)
		{
			if (handle_operator(ctx) == -1)
				return (-1);
		}
		else
		{
			if (handle_command(ctx->line,
					ctx->p, ctx->codes, ctx->env) == -1)
				return (-1);
		}
	}
	finalize_pending(ctx->p);
	return (0);
}

```

### `parse/parser_core2.c`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_core2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/17 18:01:22 by oscar             #+#    #+#             */
/*   Updated: 2026/03/21 05:36:27 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

static int	error_pipe(t_parser *p, t_intvec *codes)
{
	write(2, "syntax error near unexpected token `|'\n", 40);
	return (parser_error(p, codes));
}

static int	handle_op_line(t_parse_ctx *ctx)
{
	if (ctx->p->op_code == 5)
	{
		if (ctx->p->last_pipe)
			return (error_pipe(ctx->p, ctx->codes));
		ctx->p->last_pipe = 1;
	}
	else
		ctx->p->last_pipe = 0;
	if (handle_operator(ctx) == -1)
		return (parser_error(ctx->p, ctx->codes));
	return (0);
}

static int	handle_cmd_line(t_parse_ctx *ctx)
{
	ctx->p->last_pipe = 0;
	if (handle_command(ctx->line,
			ctx->p, ctx->codes, ctx->env) == -1)
		return (parser_error(ctx->p, ctx->codes));
	return (0);
}

static int	parse_loop_line(t_parse_ctx *ctx)
{
	ctx->p->last_pipe = 1;
	while (ctx->line[ctx->p->i])
	{
		skip_spaces(ctx->line, &ctx->p->i);
		if (!ctx->line[ctx->p->i])
			break ;
		ctx->p->op_match = match_op(ctx->line,
				ctx->p->i, &ctx->p->op_len, &ctx->p->op_code);
		if (ctx->p->op_match == -1)
			return (parser_error(ctx->p, ctx->codes));
		if (ctx->p->op_match == 1)
		{
			if (handle_op_line(ctx))
				return (-1);
		}
		else
		{
			if (handle_cmd_line(ctx))
				return (-1);
		}
	}
	if (ctx->p->last_pipe)
		return (error_pipe(ctx->p, ctx->codes));
	return (0);
}

int	parse_line(const char *line, t_intvec *codes,
	t_cmdnode **out_list, char **env)
{
	t_parser	p;
	t_parse_ctx	ctx;

	parser_init(&p, out_list);
	ctx.line = line;
	ctx.p = &p;
	ctx.codes = codes;
	ctx.env = env;
	ctx.data = out_list;
	if (parse_loop_line(&ctx) == -1)
		return (-1);
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
/*   Updated: 2026/03/21 04:59:10 by oscar            ###   ########.fr       */
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
	node->is_heredoc = 0;
	return (node);
}

t_list_red	*red_list_push_back(t_list_red **head, const char *s, int doble)
{
	t_list_red	*tail;
	t_list_red	*new;

	tail = *head;
	if (tail)
	{
		while (tail->next)
			tail = tail->next;
	}
	new = red_node_new(s, doble);
	if (!*head)
		*head = new;
	else
	{
		tail->next = new;
		new->prev = tail;
	}
	return (new);
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
/*   Updated: 2026/03/19 16:20:23 by oscar            ###   ########.fr       */
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
/*   Updated: 2026/03/21 04:54:42 by oscar            ###   ########.fr       */
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
				59);
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
				59);
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
/*   Updated: 2026/03/21 05:38:15 by oscar            ###   ########.fr       */
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
		change_exit_value("2", &env);
		return (NULL);
	}
	expanded = expand_env_vars(target, env);
	free(target);
	return (expanded);
}

void	redir_add_back(t_list_red **lst, char *content, int doble)
{
	t_list_red	*new;
	t_list_red	*tmp;

	if (!content)
		return ;
	new = malloc(sizeof(t_list_red));
	if (!new)
		return ;
	new->content = content;
	new->doble = doble;
	new->next = NULL;
	if (*lst == NULL)
	{
		*lst = new;
		return ;
	}
	tmp = *lst;
	while (tmp->next != NULL)
		tmp = tmp->next;
	tmp->next = new;
}

static int	handle_pipe(t_parser *p)
{
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
	p->current_cmd = node_new(NULL);
	cmd_list_append(&p->head, &p->tail, p->current_cmd);
	return (0);
}

static int	handle_redirection(t_parse_ctx *ctx)
{
	char	*target;

	target = get_redir_target(ctx->line, &ctx->p->i, ctx->env);
	if (!target)
		return (-1);
	if (ctx->p->current_cmd)
	{
		if (add_redir_target(ctx->p->current_cmd,
				ctx->p->op_code, target, ctx->env))
		{
			free(target);
			return (-1);
		}
	}
	else
	{
		if (store_pending_redir(ctx, target))
		{
			free(target);
			return (-1);
		}
	}
	free(target);
	return (0);
}

int	handle_operator(t_parse_ctx *ctx)
{
	intvec_push(ctx->codes, ctx->p->op_code);
	ctx->p->i += ctx->p->op_len;
	if (ctx->p->op_code == 5)
		return (handle_pipe(ctx->p));
	return (handle_redirection(ctx));
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
/*   Updated: 2026/03/21 05:35:43 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

static int	handle_heredoc(t_cmdnode *cmd,
	const char *target, char **env)
{
	char		*real_delim;
	t_list_red	*node;

	(void)env;
	real_delim = remove_quotes_str(target);
	if (!real_delim)
		return (-1);
	node = red_list_push_back(&cmd->infiles, real_delim, 1);
	if (!node)
	{
		free(real_delim);
		return (-1);
	}
	node->is_heredoc = 1;
	free(real_delim);
	return (0);
}

static int	init_heredoc_ctx(t_heredoc_fork *ctx, t_heredoc_args *args)
{
	char	*real_delim;

	ctx->expand = 1;
	if (strchr(args->target, '\'') || strchr(args->target, '"'))
		ctx->expand = 0;
	real_delim = remove_quotes_str(args->target);
	if (!real_delim)
		return (-1);
	ctx->delimiter = real_delim;
	ctx->env = args->env;
	ctx->data = args->data;
	ctx->line = args->line;
	ctx->codes = args->codes;
	return (0);
}

static int	handle_pending_heredoc(t_heredoc_args *args)
{
	t_heredoc_fork	ctx;
	char			*heredoc_content;

	if (init_heredoc_ctx(&ctx, args) == -1)
		return (-1);
	heredoc_content = heredoc_with_fork(&ctx);
	free(ctx.delimiter);
	if (!heredoc_content)
		return (0);
	red_list_push_back(&args->parser->pending_in, heredoc_content, 1);
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

int	store_pending_redir(t_parse_ctx *ctx, const char *target)
{
	t_heredoc_args	args;

	if (ctx->p->op_code == 1)
		red_list_push_back(&ctx->p->pending_in, target, 0);
	else if (ctx->p->op_code == 3)
	{
		args.parser = ctx->p;
		args.target = target;
		args.env = ctx->env;
		args.data = ctx->data;
		args.line = (char *)ctx->line;
		args.codes = ctx->codes;
		return (handle_pending_heredoc(&args));
	}
	else if (ctx->p->op_code == 2)
		red_list_push_back(&ctx->p->pending_out, target, 0);
	else if (ctx->p->op_code == 4)
		red_list_push_back(&ctx->p->pending_out, target, 1);
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
/*   Updated: 2026/03/19 16:18:00 by oscar            ###   ########.fr       */
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
/*   Updated: 2026/03/19 16:17:24 by oscar            ###   ########.fr       */
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

### `minishell_main.c`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell_main.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 23:52:06 by oscar             #+#    #+#             */
/*   Updated: 2026/03/21 04:53:23 by oscar            ###   ########.fr       */
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

```

### `minishell.h`

```cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 13:34:51 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/21 07:51:12 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <errno.h>
# include <fcntl.h>
# include <signal.h>
# include <ctype.h>
# include <termios.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/wait.h>

typedef struct s_list
{
	void			*content;
	struct s_list	*prev;
	struct s_list	*next;
}					t_list;

typedef struct s_list_red
{
	void				*content;
	int					doble;
	int					is_heredoc;
	struct s_list_red	*prev;
	struct s_list_red	*next;
}					t_list_red;

typedef struct s_cmdnode
{
	char				*cmd;
	t_list				*phrase;
	t_list_red			*infiles;
	t_list_red			*outfiles;
	struct s_cmdnode	*next;
}						t_cmdnode;

typedef struct s_intvec
{
	int		*data;
	size_t	len;
	size_t	cap;
}			t_intvec;

typedef struct s_exit_ctx
{
	t_list		*args;
	int			in_child;
	char		***envp;
	t_cmdnode	**data;
	char		*line;
	t_intvec	*codes;
}	t_exit_ctx;

typedef struct s_parser
{
	size_t		i;
	size_t		j;
	size_t		op_len;
	int			op_code;
	int			op_match;
	int			last_pipe;
	t_cmdnode	*head;
	t_cmdnode	*tail;
	t_cmdnode	*current_cmd;
	t_list_red	*pending_in;
	t_list_red	*pending_out;
}	t_parser;

typedef struct s_heredoc_args
{
	t_parser	*parser;
	const char	*target;
	char		**env;
	t_cmdnode	**data;
	char		*line;
	t_intvec	*codes;
}	t_heredoc_args;

typedef struct s_parse_ctx
{
	const char	*line;
	t_parser	*p;
	t_intvec	*codes;
	char		**env;
	t_cmdnode	**data;
}	t_parse_ctx;

typedef struct s_exec_ctx
{
	char		**envp;
	t_cmdnode	**data;
	char		*line;
	t_intvec	*codes;
	int			**arr_fds;
	int			len;
}	t_exec_ctx;

typedef struct s_exec_pack
{
	char		**envp;
	t_cmdnode	**data;
	char		*line;
	t_intvec	*codes;
	int			**arr_fds;
	int			len;
}	t_exec_pack;

extern volatile sig_atomic_t	g_sigint_received;

int		builtin_echo(t_list *args);
int		builtin_pwd(void);
int		builtin_export(t_list *args, char ***envp);
int		builtin_unset(t_list *args, char ***envp);
int		builtin_env(char **envp);
int		builtin_exit(t_exit_ctx *ctx);
int		builtin_cd(t_list *args, char **envp);
void	change_exit_value(char *args, char ***envp);

char	*read_heredoc_input(const char *delimiter,
			int expand_vars, char **shell_env);
char	*remove_quotes_str(const char *str);
int		parse_line(const char *line, t_intvec *out_codes,
			t_cmdnode **out_list, char **shell_env);
char	*get_env_value(char **envp, const char *key);
char	**dup_envp(char **envp);
void	free_envp_copy(char **envp);

t_list	*ft_lstnew(void *content);
void	ft_lstadd_back(t_list **lst, t_list *new);
void	*ft_malloc(size_t size);
void	ft_free_malloc(void);
size_t	env_len(char **envp);

char	**ft_split(char const *s, char c);
size_t	ft_strlen(const char *s);
size_t	ft_strlcpy(char *dst, const char *src, size_t size);
int		find_chr(const char *s, int c);
char	*strchr_modif(const char *s, int c);
char	*ft_strjoin_modif(char const *s1, char const *s2);
char	*ft_strjoin(char const *s1, char const *s2);
int		ft_strncmp(const char *s1, const char *s2, size_t n);
char	*ft_itoa(int n);
void	*ft_memset(void *s, int c, size_t n);
char	*ft_strdup(const char *s);
void	*ft_memcpy(void *dest, const void *src, size_t n);
void	*ft_realloc(void *ptr, size_t old_size, size_t new_size);
int		ft_strcmp(const char *s1, const char *s2);
char	*ft_strndup(const char *s, size_t n);
int		ft_isdigit(int c);
int		ft_atoi(const char *nptr);

char	*get_cmd_path(t_exec_pack *p, char *cmd_name);
int		execute_cmds(t_cmdnode **data, char ***envp,
			char *line, t_intvec *codes);
void	close_fds(int **arr_fds, int len_cmds);

int		is_built_in(char *cmd_name);

void	handle_sigint(int sig);

void	handle_sigint_heredoc(int sig);
void	setup_signals_heredoc(void);

void	cmd_list_destroy(t_cmdnode *h);
void	intvec_destroy(t_intvec *v);
void	free_envp_copy(char **envp);

#endif

//pasar norminette
// "" | ls
//ls | | ls
//exit 999999999999999999999
```

