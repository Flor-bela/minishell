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
