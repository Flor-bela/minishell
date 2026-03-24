/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkalgano <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 06:04:33 by oscar             #+#    #+#             */
/*   Updated: 2026/03/24 18:03:14 by dkalgano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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
		freedom((void **)&cwd);
		cwd = NULL;
		if (errno != ERANGE)
			return (1);
		size *= 2;
	}
	printf("%s\n", cwd);
	freedom((void **)&cwd);
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
