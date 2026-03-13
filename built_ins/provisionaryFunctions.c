#include "../minishell.h"

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
	len = strlen(s);
	dup = (char *)malloc(len + 1);
	if (!dup)
		return (NULL);
	memcpy(dup, s, len);
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
		if (strncmp(envp[i], key, klen) == 0
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
	len = strlen(key);
	i = 0;
	while (envp[i])
	{
		if (strncmp(envp[i], key, len) == 0 && envp[i][len] == '=')
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
		res = strdup(s2);
		if (!res)
			return (NULL);
		return (res);
	}
	len1 = strlen(s1);
	len2 = strlen(s2);
	res = malloc(len1 + len2 + 1);
	if (!res)
	{
		free(s1);
		return (NULL);
	}
	memcpy(res, s1, len1);
	memcpy(res + len1, s2, len2);
	res[len1 + len2] = '\0';
	free(s1);
	return (res);
}

char	*remove_quotes_str(const char *str)
{
	size_t	i;
	size_t	j;
	size_t	len;
	char	quote;
	char	*res;

	if (!str)
		return (NULL);
	len = strlen(str);
	res = malloc(len + 1);
	if (!res)
		return (NULL);
	i = 0;
	j = 0;
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
				res[j++] = str[i];
		}
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
	len = strlen(base);
	res = malloc(len + 2);
	if (!res)
	{
		free(base);
		return (NULL);
	}
	memcpy(res, base, len);
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
		return (strdup(add));
	len1 = strlen(base);
	len2 = strlen(add);
	res = malloc(len1 + len2 + 1);
	if (!res)
	{
		free(base);
		return (NULL);
	}
	memcpy(res, base, len1);
	memcpy(res + len1, add, len2);
	res[len1 + len2] = '\0';
	free(base);
	return (res);
}

static char	*expand_heredoc_line(const char *line, char **shell_env)
{
	size_t	i;
	size_t	start;
	char	*res;
	char	*var_name;
	char	*value;

	i = 0;
	res = strdup("");
	if (!res)
		return (NULL);
	while (line[i])
	{
		if (line[i] == '$')
		{
			i++;
			if (!line[i])
			{
				res = append_char_local(res, '$');
				break ;
			}
			if (line[i] == '?')
			{
				value = get_env_value(shell_env, "return_value");
				if (value)
					res = append_str_local(res, value);
				i++;
			}
			else if (!is_var_start_local(line[i]))
				res = append_char_local(res, '$');
			else
			{
				start = i;
				while (line[i] && is_var_char_local(line[i]))
					i++;
				var_name = strndup(line + start, i - start);
				if (!var_name)
				{
					free(res);
					return (NULL);
				}
				value = get_env_value(shell_env, var_name);
				free(var_name);
				if (value)
					res = append_str_local(res, value);
			}
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

char	*read_heredoc_input(const char *delimiter, int expand_vars, char **shell_env)
{
	char	*line;
	char	*content;
	char	*tmp;
	char	*expanded;

	content = strdup("");
	if (!content)
		return (NULL);
	while (1)
	{
		line = readline("> ");
		if (!line)
			break ;
		if (strcmp(line, delimiter) == 0)
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
			{
				free(content);
				return (NULL);
			}
		}
		tmp = join_and_free(content, line);
		if (!tmp)
		{
			free(line);
			return (NULL);
		}
		content = tmp;
		tmp = join_and_free(content, "\n");
		if (!tmp)
		{
			free(line);
			return (NULL);
		}
		content = tmp;
		free(line);
	}
	return (content);
}

int	builtin_echo_remake(t_list *args)
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

void	change_exit_value(char *args, char ***envp)
{
	int		idx;

	idx = find_env_index(*envp, args);
	if (idx >= 0)
		replace_env_value(*envp, idx, args);
	else
		append_env_value(envp, args);
	return ;
}

int	builtin_export(t_list *args, char ***envp)
{
	t_list	*curr;
	char	*arg;
	int		idx;

	if (!args)
	{
		print_export_env(*envp);
		return (0);
	}
	curr = args;
	while (curr)
	{
		arg = (char *)curr->content;
		if (!is_valid_identifier(arg))
		{
			fprintf(stderr, "export: `%s': not a valid identifier\n", arg);
			curr = curr->next;
			continue ;
		}
		if (!strchr(arg, '='))
		{
			curr = curr->next;
			continue ;
		}
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
			fprintf(stderr, "unset: `%s': not a valid identifier\n", arg);
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

int	builtin_exit(t_list *args)
{
	(void)args;
	return (1);
}