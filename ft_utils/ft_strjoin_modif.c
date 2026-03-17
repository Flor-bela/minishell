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
