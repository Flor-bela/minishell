NAME = minishell

SOURCES = count_cmds.c count_pipes.c ft_lstnew.c ft_lstadd_back.c ft_mallocs_list.c\
		main.c


OBJECTS = $(SOURCES:.c=.o)

FLAGS = -Wall -Werror -Wextra

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