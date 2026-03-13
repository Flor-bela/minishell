NAME = minishell

SOURCES = ft_utils/ft_lstnew.c ft_utils/ft_lstadd_back.c ft_utils/ft_mallocs_list.c\
		ft_utils/ft_split.c ft_utils/ft_strlen.c ft_utils/ft_strlcpy.c execution/execution.c\
		built_teams/provisionaryFunctions.c find_chr.c strchr_modif.c ft_utils/ft_strjoin.c\
		ft_utils/ft_strncmp.c get_cmd_path.c close_fds.c minishell_main.c parse/parse.c is_built_in.c

OBJ_DIR = obj
OBJECTS = $(SOURCES:%.c=$(OBJ_DIR)/%.o)

FLAGS = -Wall -Werror -Wextra -g
LDFLAGS = -lreadline -lhistory
CC = cc

all: $(NAME)

$(NAME): $(OBJECTS)
	$(CC) $(FLAGS) $(OBJECTS) $(LDFLAGS) -o $(NAME)

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re