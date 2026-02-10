/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built_in_echo.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fda-roch <<fda-roch@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/03 17:47:05 by fda-roch          #+#    #+#             */
/*   Updated: 2026/02/09 18:25:02 by fda-roch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// necesitamos opcion con -n (no hacer newline después)
// no estoy segura donde vamos a poner esa informacion de si tiene -n o no...

int	builtin_echo(char **args)
{
	int	i;
	
	i = 1;
	if (args == NULL || args[0] == NULL)
	{
		printf("\n");
		return (0); // escribir echo sin nada más devulve salto de línea y salida de 0
	}
	
	while (args[i] != NULL)
	{
		printf("%s ", args[i]);
		i++;
	}
	printf("\n");
	return (0); // exit code is 0
}

/* int	main(int argc, char **argv)
{
	if (argc == 1)
		return (1);
	builtin_echo(argv);
	return (0);
} */
