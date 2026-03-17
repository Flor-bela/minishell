/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 16:18:45 by fda-roch          #+#    #+#             */
/*   Updated: 2026/03/17 12:59:42 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static size_t	count_words(char const *s, char c)
{
	int		i;
	int		words;
	size_t	len_s;

	i = 0;
	words = 0;
	len_s = ft_strlen(s);
	while (s[i] != '\0')
	{
		if (s[i] == c && s[i + 1] != c)
			words++;
		i++;
	}
	if (s[len_s - 1] == c && len_s != 0)
		words--;
	if (s[0] == c && c != '\0')
		words--;
	if (s[0] != '\0')
		words++;
	return (words);
}

static void	find_start_end(char const **s, char c, char **start, char **end)
{
	*start = *end;
	while ((**s != '\0') && (*start == *end))
	{
		if (**s == c)
			(*s)++;
		if (**s != c)
			*start = (char *)*s;
		(*s)++;
	}
	*end = *start;
	while (*end == *start)
	{
		if ((**s == c) || (**s == '\0'))
			*end = (char *)*s;
		(*s)++;
	}
}

/* static char	**clean(char **arr, int j)
{
	int	i;

	i = 0;
	while (i < j)
	{
		free (arr[i]);
		i++;
	}
	free (arr);
	return (NULL);
} */

char	**ft_split(char const *s, char c)
{
	char	**arr;
	int		j;
	int		words;
	char	*start;
	char	*end;

	if (s == NULL)
		return (NULL);
	j = -1;
	start = NULL;
	end = NULL;
	words = count_words(s, c);
	arr = (char **)ft_malloc(sizeof(char *) * (words + 1));
	arr[words] = NULL;
	while (++j < words)
	{
		find_start_end(&s, c, &start, &end);
		arr[j] = (char *)ft_malloc(end - start + 1);
		ft_strlcpy(&arr[j][0], start, (int)(end - start + 1));
	}
	return ((char **)arr);
}
