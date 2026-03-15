NAME = minishell

SOURCES = \
	built_ins/provisionaryFunctions.c \
	\
	execution/execution.c \
	execution/find_chr.c \
	execution/strchr_modif.c \
	execution/get_cmd_path.c \
	execution/close_fds.c \
	execution/is_built_in.c \
	\
	parse/parse.c \
	\
	ft_utils/ft_lstnew.c \
	ft_utils/ft_lstadd_back.c \
	ft_utils/ft_mallocs_list.c \
	ft_utils/ft_split.c \
	ft_utils/ft_strlen.c \
	ft_utils/ft_strlcpy.c \
	ft_utils/ft_strjoin.c \
	ft_utils/ft_strncmp.c \
	ft_utils/ft_itoa.c \
	ft_utils/ft_memset.c\
	ft_utils/ft_strdup.c\
	ft_utils/ft_memcpy.c\
	\
	minishell_main.c

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