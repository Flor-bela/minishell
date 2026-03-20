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
/*   Updated: 2026/03/19 22:39:12 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "provisionaryFunctions.h"

//int	builtin_cd(char **args, **envp)
 
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

int	builtin_cd(t_list *args, char **envp)
{
	char	*path;
	char	*oldpwd;
	char	*newpwd;

	oldpwd = NULL;
	newpwd = NULL;
	if (args != NULL && args->next != NULL)
	{
		write(2, "minishell: cd: too many arguments\n", 35);
		return (1);
	}
	oldpwd = getcwd(NULL, 0);
	if (envp == NULL)
		return (1);
	if (oldpwd == NULL)
		oldpwd = ft_strdup("");
	if (args == NULL)
	{
		path = get_env_value(envp, "HOME");
		if (path == NULL)
		{
			write(2, "minishell: cd: HOME not set\n", 29);
			free(oldpwd);
			return (1);
		}
	}
	else
		path = args->content;
	if (chdir(path) != 0)
	{
		perror("minishell: cd");
		free(oldpwd);
		return (1);
	}
	newpwd = getcwd(NULL, 0);
	if (newpwd == NULL)
		newpwd = ft_strdup("");
	update_env(envp, "OLDPWD", oldpwd);
	update_env(envp, "PWD", newpwd);
	free(oldpwd);
	free(newpwd);
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
/*   Updated: 2026/03/19 22:50:12 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PROVISIONARYFUNCTIONS_H
# define PROVISIONARYFUNCTIONS_H

# include "../minishell.h"

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
int		builtin_exit(t_list *args, int in_child);
int		builtin_cd(t_list *args, char **envp);

void	change_exit_value(char *args, char ***envp);
char	*heredoc_with_fork(const char *delimiter,
			int expand_vars, char **env);

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
/*   Updated: 2026/03/20 00:40:15 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "provisionaryFunctions.h"

static size_t	env_len(char **envp)
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

static int	append_env_value(char ***envp, const char *new_value)
{
	size_t	len;
	char	**new_env;
	size_t	i;

	len = env_len(*envp);
	new_env = (char **)malloc(sizeof(char *) * (len + 2));
	if (!new_env)
		return (1);
	i = 0;
	while (i < len)
	{
		new_env[i] = (*envp)[i];
		i++;
	}
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

static int	remove_env_index(char ***envp, int idx)
{
	size_t	len;
	char	**new_env;
	size_t	i;
	size_t	j;

	if (!envp || !*envp || idx < 0)
		return (0);
	len = env_len(*envp);
	new_env = (char **)malloc(sizeof(char *) * len);
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
	
	////printf("DEBUG join_and_free s1=%p s2=%p\n", s1, s2);
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

/*static int	process_heredoc_line(char **line, char **content,
	int expand_vars, char **shell_env)
{
	char	*tmp;
	char	*expanded;

	if (expand_vars)
	{
		expanded = expand_heredoc_line(*line, shell_env);
		free(*line);
		*line = expanded;
		if (!*line)
			return (1);
	}
	tmp = join_and_free(*content, *line);
	if (!tmp)
		return (1);
	*content = tmp;
	tmp = join_and_free(*content, "\n");
	if (!tmp)
		return (1);
	*content = tmp;
	return (0);
}*/

static char *get_next_line(int fd)
{
    char *buf = malloc(1024);
    int i = 0;
    char c;

    if (!buf)
        return NULL;

    while (read(fd, &c, 1) > 0)
    {
        buf[i++] = c;
        if (c == '\n')
            break;
    }

    if (i == 0)
    {
        free(buf);
        return NULL;
    }

    buf[i] = '\0';
    return buf;
}

char	*read_heredoc_input(const char *delimiter,
			int expand_vars, char **shell_env)
{
	char	*line;
	char	*content;
	char	*tmp;
	char	*expanded;

	content = ft_strdup("");
	if (!content)
		return (NULL);

	g_sigint_received = 0;
	setup_signals_heredoc();

	while (1)
	{
		write(1, "> ", 2);
		line = get_next_line(STDIN_FILENO);

		if (g_sigint_received)
		{
			g_sigint_received = 0;

			free(line);
			free(content);

			return (NULL);
		}

		if (!line)
			break ;

		if (line)
		{
			int len = ft_strlen(line);
			if (len > 0 && line[len - 1] == '\n')
				line[len - 1] = '\0';
		}
		if (ft_strcmp(line, delimiter) == 0)
		{
			free(line);
			break ;
		}

		if (expand_vars)
		{
			expanded = expand_heredoc_line(line, shell_env);
			free(line);
			line = expanded;
			if (!line)
				return (NULL);
		}

		tmp = join_and_free(content, line);
		if (!tmp)
			return (NULL);
		content = tmp;

		tmp = join_and_free(content, "\n");
		if (!tmp)
			return (NULL);
		content = tmp;

		free(line);
	}

	return (content);
}

char	*read_fd_all(int fd)
{
	char	buffer[1025];
	char	*result;
	char	*tmp;
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

		tmp = join_and_free(result, buffer);
		if (!tmp)
		{
			free(result);
			return (NULL);
		}
		result = tmp;
	}
	return (result);
}

char	*heredoc_with_fork(const char *delimiter,
			int expand_vars, char **env)
{
	int		fd[2];
	pid_t	pid;
	char	*content;
	int		status;

	////printf("DEBUG entro heredoc\n");
	if (pipe(fd) == -1)
		return (NULL);

	pid = fork();
	if (pid == -1)
	{
		////printf("DEBUG padre heredoc\n");
		
		return (NULL);
	}

	if (pid == 0)
	{
		////printf("DEBUG hijo heredoc\n");
		close(fd[0]);

		content = read_heredoc_input(delimiter, expand_vars, env);

		if (!content)
		{
			close(fd[1]);
			exit(130); // Ctrl+C
			//return 1;
		}

		write(fd[1], content, ft_strlen(content));
		free(content);
		close(fd[1]);
		exit(0);
	}

	// 🔥 PARENT
	close(fd[1]);

	waitpid(pid, &status, 0);

	////printf("DEBUG status raw = %d\n", status);
	////printf("DEBUG WIFEXITED=%d WEXITSTATUS=%d\n",
    WIFEXITED(status), WEXITSTATUS(status));
	////printf("DEBUG WIFSIGNALED=%d WTERMSIG=%d\n",
    WIFSIGNALED(status), WTERMSIG(status));

	// 🔥 🔥 🔥 CLAVE
	if (g_sigint_received == 1 ||
    (WIFEXITED(status) && WEXITSTATUS(status) == 130) ||
    (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT))
	{
		////printf("DEBUG heredoc cancelado\n");
		
		close(fd[0]);
		g_sigint_received = 1;
		return (NULL);
	}

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

static int	process_export_arg(char *arg, char ***envp)
{
	int	idx;

	if (!is_valid_identifier(arg))
	{
		write(2, "export: `", 9);
		write(2, arg, ft_strlen(arg));
		write(2, "': not a valid identifier\n", 26);
		return (0);
	}
	if (!strchr(arg, '='))
		return (0);
	idx = find_env_index(*envp, arg);
	if (idx >= 0)
	{
		if (replace_env_value(*envp, idx, arg) != 0)
			return (1);
	}
	else
	{
		if (append_env_value(envp, arg) != 0)
			return (1);
	}
	return (0);
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
			continue;
		}

		arg = (char *)curr->content;

		if (process_export_arg(arg, envp))
			return (1);

		curr = curr->next;
	}
	return (0);
}

int	builtin_unset(t_list *args, char ***envp)
{
	t_list	*curr;
	char	*arg;
	int		idx;

	curr = args;
	while (curr)
	{
		arg = (char *)curr->content;
		if (!is_valid_identifier(arg))
		{
			write(2, "unset: `", 8);
			write(2, arg, ft_strlen(arg));
			write(2, "': not a valid identifier\n", 26);
			curr = curr->next;
			continue ;
		}
		idx = find_env_index(*envp, arg);
		if (idx >= 0)
		{
			if (remove_env_index(envp, idx) != 0)
				return (1);
		}
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

int	builtin_exit(t_list *args, int in_child) // todos tienen aunque sea error que hacer exit!!!
{
	if (args == NULL) // si sale deberia liberar todo aqui!!!
		exit (0); ///asi por ahora
	
	int	i;
	t_list	*tmp;
	
	i = 0;
	tmp = args;
	while (tmp != NULL)
	{
		if (tmp->content != NULL)
			i++;
		tmp = tmp->next;
	}
	// primero ver si son non-digit!!!! da ese error antes de que son muchos argumentos!
	char	*str;
	int		j;
	int		flag;

	tmp = args;
	str = tmp->content;
	j = 0;
	flag = 0;

	while (str[j] != '\0')
	{
		if (str[0] == '-' || str[0] == '+') // si es negativo continuar
			j++;
		if ((ft_isdigit(str[j]) == 0)) // significa que no es digito!!!
		{
			flag = 1;
			break;
		}
		j++;
	}
	if (flag == 1) // error numeric argument required -> return (2); -> primero error
	{
		char *error;
		if (in_child != 1)
			write(1, "exit\n", 6);
		error = ft_strjoin_modif("minishell: exit: ", str);
		error = ft_strjoin_modif(error, ": numeric argument required\n");
		write(2, error, ft_strlen(error));
		
		// si sale deberia liberar todo aqui!!!
		exit (2); /// aqui si sale!
	} 
 	if (i > 1)
	{
		if (in_child != 1)
			write(1, "exit\n", 6);
		write(2, "minishell: exit: too many arguments\n", 37);
		return (1);
	}
	
	// si llega aqui es que no tuvo errores antes!
	int	num;
	
	num = ft_atoi((const char*)str);
	if (num < 0 || num > 255)
		num  = num % 256;
	if (in_child != 1)
			write(1, "exit\n", 6);
	exit (num);
}

void	change_exit_value(char *value, char ***envp)
{
	int		idx;
	char	*entry;

	//printf("DEBUG entro change_exit_value: %s\n", value);
	if (!value)
		return ;
	if(value==0)
		entry = "return_value=0";
	else
		entry = ft_strjoin("return_value=", value);
	//printf("DEBUG entro change_exit_value: %s\n", entry);
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
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 17:56:01 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/18 19:28:55 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	close_fds(int **arr_fds, int len_cmds)
{
	int	j;

	j = 0;
	if (arr_fds == NULL)
		return;
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
/*   Created: 2026/02/10 15:35:41 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/20 00:31:21 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
#include "../built_ins/provisionaryFunctions.h"

static void	error_fork(char **envp)
{
	perror("minishell: fork"); // probar
	ft_free_malloc(); // ??
	change_exit_value("1", &envp);
	exit(1);
}

static void	error_pipe(char **envp)
{
	perror("minishell: pipe"); // probar
	ft_free_malloc(); // ??
	change_exit_value("1", &envp);
	exit(1);
}

 
static void	error_fd_file(char *name_infile, char **envp)
{
	char *error;
	
	error = ft_strjoin_modif("minishell: ", name_infile);
	perror(error);
	ft_free_malloc();
	change_exit_value("1", &envp);
	exit(1);
}


char	**get_args(t_list	*phrase)
{
	//1st count how many arguments we have (t_list->content)
	t_list	*tmp;
	int		args_count;
	char	**args;
	int		i;

	args_count = 0;
	i = 0;
	args = NULL;
	tmp = phrase;
	while (tmp != NULL)
	{
		args_count++;
		tmp = tmp->next;
	}
	args = (char **)ft_malloc(sizeof(char *) * (args_count + 2)); // + 2 para añadir el array con nombre del comando (args[0]) y el ultimo array que es el del caracter nulo
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

static char	*get_cmd_name_and_path(char **args, char **envp, char *data_cmd, int **arr_fds, int len) // adds args[0] has the name of the command and gets command path
{
	char	*cmd_name;
	char	*cmd_path;
	
	cmd_path = NULL;
	if (find_chr(data_cmd, '/') == 1) // if it has / in the name of the command...
	{
		cmd_name = strchr_modif(data_cmd, '/');
		cmd_path = data_cmd;
		if (access(cmd_path, F_OK) != 0)
		{
			char *error;
			
			error = ft_strjoin_modif("minishell: ", data_cmd);
			perror(error);
			close_fds(arr_fds, len);
			ft_free_malloc();
			change_exit_value("127", &envp);
			exit(127);
		}
		if (access(cmd_path, X_OK) != 0)
		{
			char *error;
			
			error = ft_strjoin_modif("minishell: ", data_cmd);
			perror(error);
			close_fds(arr_fds, len);
			ft_free_malloc();
			change_exit_value("126", &envp);
			exit(126);
		}
		//if (access(cmd_path, X_OK) == 0)
		args[0] = cmd_name;

	}
	else //-> si no tiene / entonces busca el path del comando y añade el nombre directamente a args[0];
	{
		cmd_path = get_cmd_path(envp, data_cmd, arr_fds, len);
		args[0] = data_cmd;
	}
	return (cmd_path);
}

static void handle_infile_redirections(t_list_red **tmp_infiles,
    int **arr_fds, int len, char **envp)
{
    int fd_file;

    while (*tmp_infiles != NULL)
    {
        if ((*tmp_infiles)->doble == 1)
        {
            // 🔥 HEREDOC
            char *content = heredoc_with_fork((*tmp_infiles)->content, 1, envp);

            if (!content)
                exit(1);

            int pipefd[2];
            pipe(pipefd);

            write(pipefd[1], content, ft_strlen(content));
            close(pipefd[1]);

            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);

            free(content);
        }
        else
        {
            // 🔥 ARCHIVO NORMAL
            fd_file = open((*tmp_infiles)->content, O_RDONLY);
            if (fd_file == -1)
            {
                close_fds(arr_fds, len);
                error_fd_file((*tmp_infiles)->content, envp);
            }
            dup2(fd_file, STDIN_FILENO);
            close(fd_file);
        }

        *tmp_infiles = (*tmp_infiles)->next;
    }
}

static void	handle_outfile_redirections(t_list_red **tmp_outfiles,
	int **arr_fds, int len, char **envp)
{
	int	fd_file;

	while (*tmp_outfiles != NULL)
	{
		if ((*tmp_outfiles)->doble == 0)
		{
			// >
			fd_file = open((*tmp_outfiles)->content,
				O_WRONLY | O_CREAT | O_TRUNC, 0666);
		}
		else
		{
			// >>
			fd_file = open((*tmp_outfiles)->content,
				O_WRONLY | O_CREAT | O_APPEND, 0666);
		}

		if (fd_file == -1)
		{
			close_fds(arr_fds, len);
			error_fd_file((*tmp_outfiles)->content, envp);
		}

		dup2(fd_file, STDOUT_FILENO);
		close(fd_file);

		*tmp_outfiles = (*tmp_outfiles)->next;
	}
}

int	execute_cmds(t_cmdnode **data, char **envp)
{
	pid_t	*pids;
	char	**args;
	char	*cmd_path;
	int		len;
	int		i;
	int		last_exit_status;
	int		**arr_fds;
	t_cmdnode *data_tmp;

	////printf("DEBUG entrando execute_cmds\n");
	len = 0;
	i = 0;
	last_exit_status = 0;
	data_tmp = *data;
	pids = NULL;
	arr_fds = NULL;
	//while (data_tmp && data_tmp->cmd != NULL) // data && data_tmp->cmd anterior
	while (data_tmp != NULL)
	{
		len++;
		data_tmp = data_tmp->next;
	}
	//printf("len is %d\n", len);
	//if (len > 0) // added this...in those cases where cmd is equal == 0
	//printf("len is %d\n", len);
	if (len > 1) // i) added this...in those cases where cmd is equal == 0 ii)len > 0 ->
	{
		arr_fds = (int **)ft_malloc(sizeof(int *) * (len - 1)); // len has the amount of commands - 1 is the correct number for pipes
		while (i < len - 1)
		{
			arr_fds[i] = (int *)ft_malloc(sizeof(int) * 2); 
			if (pipe(arr_fds[i]) == -1)
				error_pipe(envp);
			i++;
		}
	}
	i = 0;
	data_tmp = *data;

	// 🔥 BUILTINS SIN FORK (caso simple: 1 comando)
	if (len == 1 && data_tmp->cmd != NULL)
	{
		////printf("DEBUG entrando builtins sin forks1\n");
		if (is_built_in(data_tmp->cmd))
		{
			////printf("DEBUG entrando builtins sin forks2\n");
			if (ft_strcmp(data_tmp->cmd, "echo") == 0)
				return (builtin_echo(data_tmp->phrase));
			else if (ft_strcmp(data_tmp->cmd, "pwd") == 0)
				return (builtin_pwd());
			else if (ft_strcmp(data_tmp->cmd, "env") == 0)
				return (builtin_env(envp));
			else if (ft_strcmp(data_tmp->cmd, "export") == 0)
				return (builtin_export(data_tmp->phrase, &envp));
			else if (ft_strcmp(data_tmp->cmd, "unset") == 0)
				return (builtin_unset(data_tmp->phrase, &envp));
			//else if (ft_strcmp(data_tmp->cmd, "cd") == 0)
			//	return (builtin_cd(data_tmp->phrase, envp));
			else if (ft_strcmp(data_tmp->cmd, "exit") == 0)
				return (builtin_exit(data_tmp->phrase, 0));
		}
	}

	// Only redirections without commands and without pipes!
	if (len == 1 && (data_tmp->cmd == NULL || data_tmp->cmd[0] == '\0'))// (if (len == 0)) cambie esto porque solo con infiles o outfiles, len ya va a ser == 1!!!
	{
		// infiles < 
		int	saved_stdin;
		int saved_stdout;

		saved_stdin = -1;
		saved_stdout = -1;
		if (data_tmp->infiles != NULL)
		{
			t_list_red	*tmp_infiles;
			int	fd_file;
			
			tmp_infiles = data_tmp->infiles;
			saved_stdin = dup(STDIN_FILENO);
			while (tmp_infiles != NULL)
			{
				if (tmp_infiles->doble == 1)
				{
					tmp_infiles = tmp_infiles->next;
				}else{
					fd_file = open(tmp_infiles->content, O_RDONLY);
					if (fd_file == -1)
					{
						char *error;
						
						error = ft_strjoin_modif("minishell: ", (char *)tmp_infiles->content);
						perror(error);
						return (1);
					}
					dup2(fd_file, STDIN_FILENO);
					close(fd_file);
					tmp_infiles = tmp_infiles->next;	
				}
			}
		}
		// outfiles >
		if (data_tmp->outfiles != NULL)
		{
			t_list_red	*tmp_outfiles;
			int	fd_file;
			
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
					char *error;
					
					error = ft_strjoin_modif("minishell: ", (char *)tmp_outfiles->content);
					perror(error);
					return (1);
				}
				dup2(fd_file, STDOUT_FILENO);
				close (fd_file);
				tmp_outfiles = tmp_outfiles->next;
			}
		}
		///// 
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
		return (0); // si se ejecuta bien???
	}
	
	// Only one command -> built-in!!!
	if (len == 1 && (is_built_in(data_tmp->cmd) == 1))
	{
		// infiles <
		t_list_red	*tmp_infiles;
		int 		saved_stdin;
		
		saved_stdin = -1;
		tmp_infiles = data_tmp->infiles;
		if (tmp_infiles != NULL)
		{
			int	fd_file;
			
			saved_stdin = dup(STDIN_FILENO);
			while (tmp_infiles != NULL)
			{
				fd_file = open(tmp_infiles->content, O_RDONLY);
				if (fd_file == -1)
				{
					close_fds(arr_fds, len);
					error_fd_file(tmp_infiles->content, envp);
				}
				dup2(fd_file, STDIN_FILENO);
				close(fd_file);
				tmp_infiles = tmp_infiles->next;
			}
		}
		// outfiles >
		t_list_red	*tmp_outfiles;
		int	saved_stdout;

		saved_stdout = -1;
		tmp_outfiles = data_tmp->outfiles;
		if (tmp_outfiles != NULL)
		{
			int	fd_file;
			
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
					error_fd_file(tmp_outfiles->content, envp);
				}
				dup2(fd_file, STDOUT_FILENO);
				close (fd_file);
				tmp_outfiles= tmp_outfiles->next;
			}
		}
		
		//ejecutar
		int	return_value;
		////printf("DEBUG antes de execute_cmds\n");
		return_value = 0;
		if (data_tmp && (ft_strncmp (data_tmp->cmd, "echo", 4) == 0 && data_tmp->cmd[4] == '\0'))
			return_value = builtin_echo(data_tmp->phrase);
		//else if (data_tmp && (ft_strncmp (data_tmp->cmd, "cd", 2) == 0 && data_tmp->cmd[2] == '\0'))
		//	return_value = builtin_cd(data_tmp->phrase, envp);
		else if (data_tmp && (ft_strncmp (data_tmp->cmd, "pwd", 3) == 0 && data_tmp->cmd[3] == '\0'))
			return_value = builtin_pwd();
		else if (data_tmp && (ft_strncmp (data_tmp->cmd, "export", 6) == 0 && data_tmp->cmd[6] == '\0'))
			return_value = builtin_export(data_tmp->phrase, &envp);
		else if (data_tmp && (ft_strncmp (data_tmp->cmd, "unset", 5) == 0 && data_tmp->cmd[5] == '\0'))
			return_value = builtin_unset(data_tmp->phrase, &envp);
		else if (data_tmp && (ft_strncmp (data_tmp->cmd, "env", 3) == 0 && data_tmp->cmd[3] == '\0'))
			return_value = builtin_env(envp);
		else if (data_tmp && (ft_strncmp (data_tmp->cmd, "exit", 4) == 0 && data_tmp->cmd[4] == '\0'))
			return_value = builtin_exit(data_tmp->phrase, 0);
		
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
		return(return_value);
	}
	
	else //// Pipeline!!!! 
	{
		pids = ft_malloc(sizeof(pid_t) * len); // si es solo un y built-in no deberia crear pids....(quizás esto lo pongo dentro del loop???)
		
		signal(SIGINT, handle_sigint);
		signal(SIGQUIT, SIG_IGN);

		while (i < len) // ir executando este loop para crear los procesos y ejecutar los comandos
		{
			pids[i] = fork();
			if (pids[i] == -1)
				error_fork(envp);
			else if (pids[i] == 0) // in child process
			{

				signal(SIGINT, SIG_DFL);
				signal(SIGQUIT, SIG_DFL);

				// 1st count how many arguments we have:
				if (data_tmp->cmd != NULL)
					args = get_args(data_tmp->phrase);
				else
					args = NULL;
				// 2nd: obtain the name of the command to add it do args[0] to execute with execve and gets command path
				if (data_tmp->cmd != NULL)
					cmd_path = get_cmd_name_and_path(args, envp, data_tmp->cmd, arr_fds, len);
				else
					cmd_path = NULL;
				// 3rd: check if we are in the first command, in the "middle" or in the last command (they will be different)
				if (i == 0) // first command and only one command
				{
					// infiles <
					t_list_red	*tmp_infiles;

					tmp_infiles = data_tmp->infiles;
					if (tmp_infiles != NULL)
						handle_infile_redirections(&tmp_infiles, arr_fds, len, envp);

					// outfiles >
					t_list_red	*tmp_outfiles;

					tmp_outfiles = data_tmp->outfiles;
					if (tmp_outfiles != NULL)
						handle_outfile_redirections(&tmp_outfiles, arr_fds, len, envp);
					if (len > 1 && data_tmp->outfiles == NULL) // si existe pipe escribe en el pipe, si no que escriba en el terminal
						dup2(arr_fds[0][1], STDOUT_FILENO);
				}
				else if (i != 0 && i == (len - 1)) // last command (i no puede ser 0 si no va a repetirse con el primero comando)
				{
					// infiles <
					t_list_red	*tmp_infiles;

					tmp_infiles = data_tmp->infiles;
					if (tmp_infiles != NULL)
						handle_infile_redirections(&tmp_infiles, arr_fds, len, envp);
					else // only reads from pipe if there are no infiles
						dup2(arr_fds[i - 1][0], STDIN_FILENO); // lee del pipe
					// escribe directamente en STDOUT -> o sea la terminal
					
					//outfiles >
					t_list_red	*tmp_outfiles;

					tmp_outfiles = data_tmp->outfiles;
					if (tmp_outfiles != NULL)
						handle_outfile_redirections(&tmp_outfiles, arr_fds, len, envp);
				}
				else // in the middle
				{
					t_list_red	*tmp_infiles;

					tmp_infiles = data_tmp->infiles;
					if (tmp_infiles != NULL)
						handle_infile_redirections(&tmp_infiles, arr_fds, len, envp);
					else
					{
						dup2(arr_fds[i - 1][0], STDIN_FILENO); // take the reading part of the previous pipe as standard input
						/* if (data_tmp->outfiles == NULL) // writes to pipe only if outfile doesn´t exist
							dup2(arr_fds[i][1], STDOUT_FILENO); // write in the writing part of the next pipe. */ // creo que esto está duplicado...
					}
					//outfiles >
					t_list_red	*tmp_outfiles;

					tmp_outfiles = data_tmp->outfiles;
					if (tmp_outfiles != NULL)
						handle_outfile_redirections(&tmp_outfiles, arr_fds, len, envp);
					else
						dup2(arr_fds[i][1], STDOUT_FILENO);
				}
				close_fds(arr_fds, len);


				// 🔥 FIX: no ejecutar si no hay comando (caso << sin cmd)
				if (data_tmp->cmd == NULL || data_tmp->cmd[0] == '\0')
				{
					ft_free_malloc();
					exit(0);
				}
				

				int	return_value;
				
				return_value = 0;
				// 4th: executes the command:
			
				if (data_tmp && (ft_strncmp (data_tmp->cmd, "echo", 4) == 0 && data_tmp->cmd[4] == '\0'))
					return_value = builtin_echo(data_tmp->phrase);
			//	else if (data_tmp && (ft_strncmp (data_tmp->cmd, "cd", 2) == 0 && data_tmp->cmd[2] == '\0'))
				//	return_value = builtin_cd(data_tmp->phrase, envp);
				else if (data_tmp && (ft_strncmp (data_tmp->cmd, "pwd", 3) == 0 && data_tmp->cmd[3] == '\0'))
					return_value = builtin_pwd();
				else if (data_tmp && (ft_strncmp (data_tmp->cmd, "export", 6) == 0 && data_tmp->cmd[6] == '\0'))
					return_value = builtin_export(data_tmp->phrase, &envp);
				else if (data_tmp && (ft_strncmp (data_tmp->cmd, "unset", 5) == 0 && data_tmp->cmd[5] == '\0'))
					return_value = builtin_unset(data_tmp->phrase, &envp);
				else if (data_tmp && (ft_strncmp (data_tmp->cmd, "env", 3) == 0 && data_tmp->cmd[3] == '\0'))
					return_value = builtin_env(envp);
				else if (data_tmp && (ft_strncmp (data_tmp->cmd, "exit", 4) == 0 && data_tmp->cmd[4] == '\0'))
					return_value = builtin_exit(data_tmp->phrase, 1);
				
				else // execute command:
				{
					////printf("DEBUG ejecutando comando\n");
			//		if (execve(cmd_path, args, envp) < 0)
					if (data_tmp->cmd != NULL)
						args = get_args(data_tmp->phrase);
					else
						args = NULL;
					// 2nd: obtain the name of the command to add it do args[0] to execute with execve and gets command path
					if (data_tmp->cmd != NULL)
						cmd_path = get_cmd_name_and_path(args, envp, data_tmp->cmd, arr_fds, len);
					else
						cmd_path = NULL;
					if ((cmd_path != NULL && args != NULL))
					{
						// 1st count how many arguments we have:
						
						if (execve(cmd_path, args, envp) < 0)
						{
							char *error;
						
							error = ft_strjoin_modif("minishell: ", cmd_path);
							perror(error);
							if (errno == EACCES)
							{	
								ft_free_malloc(); // ??
								change_exit_value("126", &envp);
								exit(126);
							}
							else if (errno == ENOENT)
							{
								ft_free_malloc(); // ??
								change_exit_value("127", &envp);
								exit(127);
							}
							else
							{
								ft_free_malloc(); // ??
								change_exit_value("126", &envp);
								exit(126);
							}
						}
					}
				}
				//close(arr_fds, len);
				ft_free_malloc();
				exit(return_value);
			}
			i++;
			data_tmp = data_tmp->next;
		}
		
		// Main:
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
	int signal_code;
	
	signal_code = 0;
	if (WIFEXITED(last_exit_status))
		last_exit_status = WEXITSTATUS(last_exit_status);
	else if (WIFSIGNALED(last_exit_status))
	{
		signal_code = WTERMSIG(last_exit_status);
		if (signal_code == SIGINT)
			write(1, "\n", 1);
		else if (signal_code == SIGQUIT)
			write(1, "Quit (core dumped)\n", 20);
		last_exit_status = signal_code + 128;
	}
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
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 23:02:00 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/19 12:08:29 by fda-roch         ###   ########.fr       */
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

char	*get_cmd_path(char **envp, char *cmd_name, int **arr_fds, int len)
{
	char	**paths;
	char	*cmd_path;
	char	*error;

	paths = get_all_paths(envp);
	cmd_path = find_path(paths, cmd_name);
	if (cmd_path == NULL)
	{
		if (find_chr(cmd_name, '/') == 1)
			error = ft_strjoin_modif("minishell: ", cmd_name);
		else
			error = cmd_name;
		error = ft_strjoin_modif(error, ": command not found\n");
		write(2, error, ft_strlen(error));
		close_fds(arr_fds, len);
		ft_free_malloc();
		change_exit_value("127", &envp);
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
/*   Updated: 2026/03/16 00:18:18 by fda-roch         ###   ########.fr       */
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
/*   Updated: 2026/03/20 00:15:27 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

size_t	ft_strlen(const char *s)
{
	size_t	num;

	////printf("DEBUG ft_strlen input=%p\n", s);
	num = 0;
	if (!s)
	{
		printf("ERROR: ft_strlen NULL\n");
		return (0);
	}
	////printf("DEBUG ft_strlen input=%p\n", s);
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
/*   Updated: 2026/03/19 23:09:43 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
# define PARSER_H

# include "../minishell.h"
# include "../built_ins/provisionaryFunctions.h"

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

/* list utils */
void		intvec_push(t_intvec *v, int x);
void		intvec_free(t_intvec *v);
void		str_list_push_back(t_list **head, const char *s);
void		str_list_free(t_list *lst);
t_list_red *red_list_push_back(t_list_red **head, const char *s, int doble);
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

void		parser_init(t_parser *p, t_cmdnode **out_list);
int			parser_error(t_parser *p, t_intvec *codes);
void		finalize_pending(t_parser *p);
int			parse_loop(const char *line, t_parser *p,
				t_intvec *codes, char **env);
char	*get_clean_token(const char *line, size_t *i, char **env);

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
/*   Updated: 2026/03/17 18:09:33 by oscar            ###   ########.fr       */
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

int	parse_loop(const char *line, t_parser *p,
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
/*   Updated: 2026/03/20 00:15:32 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

static int	error_pipe(t_parser *p, t_intvec *codes)
{
	write(2, "syntax error near unexpected token `|'\n", 40);
	return (parser_error(p, codes));
}

static int	handle_op_line(const char *line, t_parser *p,
	t_intvec *codes, char **env)
{
	if (p->op_code == 5)
	{
		if (p->last_pipe)
			return (error_pipe(p, codes));
		p->last_pipe = 1;
	}
	else
		p->last_pipe = 0;
	if (handle_operator(line, p, codes, env) == -1)
		return (parser_error(p, codes));
	return (0);
}

static int	handle_cmd_line(const char *line, t_parser *p,
	t_intvec *codes, char **env)
{
	p->last_pipe = 0;
	if (handle_command(line, p, codes, env) == -1)
		return (parser_error(p, codes));
	return (0);
}

static int	parse_loop_line(const char *line, t_parser *p,
	t_intvec *codes, char **env)
{
	p->last_pipe = 1;
	////printf("DEBUG entrando parser\n");
	while (line[p->i])
	{
		skip_spaces(line, &p->i);
		if (!line[p->i])
			break ;

		p->op_match = match_op(line, p->i, &p->op_len, &p->op_code);

		if (p->op_match == -1)
			return (parser_error(p, codes));

		if (p->op_match == 1)
		{
			if (handle_op_line(line, p, codes, env))
				return (-1);
		}
		else
		{
			if (handle_cmd_line(line, p, codes, env))
				return (-1);
		}
	}
	if (p->last_pipe)
		return (error_pipe(p, codes));

	return (0);
}

int	parse_line(const char *line, t_intvec *codes,
	t_cmdnode **out_list, char **env)
{
	t_parser	p;

	parser_init(&p, out_list);
	if (parse_loop_line(line, &p, codes, env) == -1)
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
/*   Updated: 2026/03/19 22:51:12 by oscar            ###   ########.fr       */
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

/*static void	red_list_append(t_list_red **head, t_list_red **tail,
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
}*/

t_list_red *red_list_push_back(t_list_red **head, const char *s, int doble)
{
	t_list_red *tail;
	t_list_red *new;

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

	return (new); // 🔥 IMPORTANTE
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
/*   Updated: 2026/03/19 16:19:53 by oscar            ###   ########.fr       */
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
/*   Updated: 2026/03/20 00:15:37 by oscar            ###   ########.fr       */
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

static int	handle_redirection(const char *line, t_parser *p, char **env)
{
	char	*target;

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


int	handle_operator(const char *line, t_parser *p,
	t_intvec *codes, char **env)
{
	////printf("DEBUG operador detectado\n");
	intvec_push(codes, p->op_code);
	p->i += p->op_len;
	if (p->op_code == 5)
		return (handle_pipe(p));
	return (handle_redirection(line, p, env));
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
/*   Updated: 2026/03/20 00:15:42 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

static int	handle_heredoc(t_cmdnode *cmd,
	const char *target, char **env)
{
	char	*real_delim;
	t_list_red *node;

	(void)env;

	real_delim = remove_quotes_str(target);
	if (!real_delim)
		return (-1);

	// ❌ NO ejecutar heredoc aquí
	// heredoc_with_fork(...)

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

	// 🔥 CAMBIO CLAVE
	heredoc_content = heredoc_with_fork(real_delim, expand_vars, env);
	////printf("DEBUG heredoc devuelto = %p\n", heredoc_content);
	free(real_delim);

	if (!heredoc_content)
		return (0);

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
/*   Updated: 2026/03/19 22:47:49 by oscar            ###   ########.fr       */
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
	int					doble; // si es doble << o >> entonces es == a 1; simples == 0;
	int 				is_heredoc;
	struct s_list_red	*prev;
	struct s_list_red	*next;
}					t_list_red;

typedef struct s_cmdnode
{
	char				*cmd;
	t_list				*phrase;
	t_list_red			*infiles; // < o << (redirecciones) -> pero yo no veo el <<
	t_list_red			*outfiles; // > o >> (redirecciones)
	struct s_cmdnode	*next;
}						t_cmdnode;

typedef struct s_intvec
{
	int		*data;
	size_t	len;
	size_t	cap;
}			t_intvec;

extern volatile sig_atomic_t g_sigint_received;

int		builtin_echo(t_list *args);
int		builtin_pwd(void);
int		builtin_export(t_list *args, char ***envp);
int		builtin_unset(t_list *args, char ***envp);
int		builtin_env(char **envp);
int		builtin_exit(t_list *args, int in_child);
int		builtin_cd(t_list *args, char **envp);
void	change_exit_value(char *args, char ***envp);

char	*read_heredoc_input(const char *delimiter, int expand_vars, char **shell_env);
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

char	*get_cmd_path(char **envp, char *cmd_name, int **arr_fds, int len);
int		execute_cmds(t_cmdnode **data, char **envp);
void	close_fds(int **arr_fds, int len_cmds);

int		is_built_in(char *cmd_name);

void	change_exit_value(char *args, char ***envp);

//int		main (int argc, char **argv, char **envp);

void	handle_sigint(int sig);

void	handle_sigint_heredoc(int sig);
void	setup_signals_heredoc(void);

#endif
```

### `minishell_main.c`

```cpp
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
	write(1, "\n", 1);
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

	g_sigint_received = 0; // 🔥 RESET AQUÍ

	line = readline(prompt);
	dprintf(2,"DEBUG: readline main devuelve = %p (%s) %d\n", line, line, g_sigint_received);
	if (g_sigint_received && (!line || *line == '\0'))
	{
		free(line);
		*out_line = NULL;
		g_sigint_received = 0;
		return (-1);
	}
	if (!line)
	{
		*out_line = NULL;
		return (0);
	}
		*out_line = line;
		return (1);
}

static char	*read_full_command()
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
	int 		*num;
	

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
		num = 0;
		write(2, "DEBUG: antes de parsear\n", 24);
		line = read_full_command(&num);
		////printf("DEBUG despues de parsear\n");
		/*if (g_sigint_received)
		{
			//////printf("DEBUG Ctrl+C detectado → salto iteración\n");
			g_sigint_received = 0;
			free(line);
			continue;
		}*/
		
		if (!line)
		{
			if (g_sigint_received && *num !=1)
			{
				g_sigint_received = 0;

				
				rl_redisplay();
				continue;
			}

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
				//////printf("DEBUG heredoc cancelado → no ejecutar\n");
				
				g_sigint_received = 0;
				cmd_list_destroy(list);
				intvec_destroy(&codes);
				free(line);
				continue;
			}*/
			//print_int_array(&codes);
			//print_nodes(list);
			exit_code = execute_cmds(&list, shell_env);
			//printf("DEBUG salio de execute_cmds:\n");	
			change_exit_value(ft_itoa(exit_code), &shell_env);
		}
		else
		{
			if (!g_sigint_received)
				write(2, "(parse error)\n", ft_strlen("(parse error)\n"));
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
```

