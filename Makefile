NAME = minishell

SOURCES = \
	\
	execution/exec_errors.c \
	execution/exec_path.c \
	execution/exec_heredoc.c \
	execution/exec_redirections.c \
	execution/exec_pipes.c \
	execution/exec_builtins.c \
	execution/exec_children.c \
	execution/exec_forks.c \
	execution/exec_utils.c \
	execution/exec_main.c \
	execution/find_chr.c \
	execution/strchr_modif.c \
	execution/get_cmd_path.c \
	execution/close_fds.c \
	execution/is_built_in.c \
	execution/execution.c\
	\
	built_ins/env_utils.c \
	built_ins/env_modify.c \
	built_ins/env_helpers.c \
	built_ins/expand_utils.c \
	built_ins/heredoc_core.c \
	built_ins/heredoc_fork.c \
	built_ins/heredoc_utils.c \
	built_ins/builtin_echo_pwd.c \
	built_ins/builtin_export.c \
	built_ins/builtin_unset_env.c \
	built_ins/builtin_exit.c \
	built_ins/builtin_cd.c \
	built_ins/str_utils.c \
	\
	parse/parser_list_redir.c \
	parse/parser_cmd_utils.c \
	parse/parser_core.c \
	parse/parser_core2.c \
	parse/parser_command.c \
	parse/parser_operator_handler.c \
	parse/parser_operator.c \
	parse/parser_redir.c \
	parse/parser_expand_utils.c \
	parse/parser_expand.c\
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