/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <fda-roch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/03 17:47:05 by fda-roch          #+#    #+#             */
/*   Updated: 2026/02/24 17:22:34 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	builtin_echo(char *phrase)
{
	int		i;
	char	**words;
	
	i = 0;
	if (phrase == NULL)
	{
		printf("\n");
		return (0); // escribir echo sin nada más devulve salto de línea y salida de 0
	}
	words = ft_split(phrase, ' ');
	if (words[0][0] == '-' && words[0][1] == 'n')
	{
		while (words[i] != NULL)
		{
			printf("%s ", words[i]);
			i++;
		}
		printf("\n");
	}
	else
	{
		while (words[i] != NULL)
		{
			printf("%s ", words[i]);
			i++;
		}
	}
	return (0); // exit code is 0
}

int	main(void)
{
	char phrase[] = "hello world";
	builtin_echo(phrase);
	return (0);
}
