NAME = minishell
SOURCES = \
	built_ins/env_utils.c \
	built_ins/env_utils2.c \
	built_ins/env_utils3.c \
	built_ins/string_utils.c \
	built_ins/expansion.c \
	built_ins/expansion2.c \
	built_ins/heredoc_core.c \
	built_ins/heredoc_core2.c \
	built_ins/heredoc_io.c \
	built_ins/heredoc_fork.c \
	built_ins/heredoc.c \
	built_ins/builtins_env.c \
	built_ins/builtin_exit_utils.c \
	built_ins/builtin_cd.c \
	built_ins/builtins.c \
	\
	execution/execution.c \
	execution/find_chr.c \
	execution/strchr_modif.c \
	execution/get_cmd_path.c \
	execution/close_fds.c \
	execution/is_built_in.c \
	\
	parse/parser_list_redir.c \
	parse/parser_cmd_utils.c \
	parse/parser_core.c \
	parse/parser_core2.c \
	parse/parser_command.c \
	parse/parser_operator_handler.c \
	parse/parser_operator.c \
	parse/parser_redir.c \
	parse/parser_expand.c \
	parse/parser_expand_utils.c \
	parse/parser_token.c \
	parse/parser_token_utils.c \
	parse/parser_list_utils.c \
	\
	ft_utils/ft_lstnew.c \
	ft_utils/ft_lstadd_back.c \
	ft_utils/ft_mallocs_list.c \
	ft_utils/ft_split.c \
	ft_utils/ft_strlen.c \
	ft_utils/ft_strlcpy.c \
	ft_utils/ft_strjoin.c \
	ft_utils/ft_strjoin_modif.c \
	ft_utils/ft_strncmp.c \
	ft_utils/ft_itoa.c \
	ft_utils/ft_memset.c \
	ft_utils/ft_strdup.c \
	ft_utils/ft_memcpy.c \
	ft_utils/ft_strndup.c \
	ft_utils/ft_strcmp.c \
	ft_utils/ft_realloc.c \
	ft_utils/ft_isdigit.c \
	ft_utils/ft_atoi.c \
	ft_utils/ft_strchr.c \
	\
	main_utils/signals.c \
	main_utils/cleanup.c \
	main_utils/input.c \
	main_utils/input2.c \
	main_utils/parse_exec.c \
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