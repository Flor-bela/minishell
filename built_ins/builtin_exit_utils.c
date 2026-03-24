/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 06:20:50 by oscar             #+#    #+#             */
/*   Updated: 2026/03/24 13:02:24 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	exit_cleanup(t_exit_ctx *ctx)
{
	ft_free_malloc();
	free_envp_copy(*ctx->envp);
	cmd_list_destroy(*ctx->data);
	freedom((void **)&ctx->line);
	intvec_destroy(ctx->codes);
}

int	count_args(t_list *args)
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

int	is_numeric(char *str)
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

void	exit_numeric_error(t_exit_ctx *ctx, char *str)
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

int	exit_too_many_args(t_exit_ctx *ctx)
{
	if (ctx->in_child != 1)
		write(1, "exit\n", 6);
	write(2, "minishell: exit: too many arguments\n", 37);
	change_exit_value("1", ctx->envp);
	return (1);
}
