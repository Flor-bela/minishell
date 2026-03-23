/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo_pwd.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 08:23:27 by oscar             #+#    #+#             */
/*   Updated: 2026/03/23 08:24:45 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "provisionaryFunctions.h"

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
