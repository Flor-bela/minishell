/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 08:29:34 by oscar             #+#    #+#             */
/*   Updated: 2026/03/23 08:31:58 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "provisionaryFunctions.h"

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