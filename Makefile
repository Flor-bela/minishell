NAME = minishell

SOURCES = 

OBJECTS = $(SOURCES:.c=.o)

CFLAGS = -Wall -Werror -Wextra




clean:
	rm -f $(OBJECTS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re