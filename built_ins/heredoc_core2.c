/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_core2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 06:13:49 by oscar             #+#    #+#             */
/*   Updated: 2026/03/24 07:05:33 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	init_heredoc_ctx(t_heredoc_ctx *ctx,
	const char *delimiter, int expand, char **env)
{
	ctx->content = ft_strdup("");
	if (!ctx->content)
		return (0);
	ctx->delimiter = delimiter;
	ctx->expand = expand;
	ctx->env = env;
	return (1);
}
