NAME = minishell

SOURCES = ft_lstnew.c ft_lstadd_back.c ft_mallocs_list.c\
		ft_split.c ft_strlen.c ft_strlcpy.c execution.c\
		find_chr.c strchr_modif.c ft_strjoin.c ft_strncmp.c\
		get_cmd_path.c close_fds.c\


OBJECTS = $(SOURCES:.c=.o)

FLAGS = -Wall -Werror -Wextra -g

CC = cc

all: $(NAME)

$(NAME): $(OBJECTS)
		$(CC) $(FLAGS) $(OBJECTS) -o $(NAME)

clean:
	rm -f $(OBJECTS)

fclean: clean
	rm -f $(NAME)

re: fclean all

%.o: %.c
	$(CC) $(FLAGS) -c $< -o $@

.PHONY: all clean fclean re