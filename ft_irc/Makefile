################################################################################
#                                     CONFIG                                   #
################################################################################

NAME = ircserv
CC = c++
INCLUDE_HEADERS = $(shell find include -name '*.hpp' | sed 's/^/-I.\//')
CFLAGS =  -g
EFLAGS = -Wall -Wextra -Werror -Weffc++
MFLAGS = -fsanitize=undefined -fno-omit-frame-pointer -fsanitize=memory
DEBUG ?= 0

CLR_RMV = \033[0m
RED	    = \033[1;31m
GREEN	= \033[1;32m
YELLOW	= \033[1;33m
BLUE	= \033[1;34m
CYAN	= \033[1;36m
RM	    = rm -rf

################################################################################
#                                    VALGRIND                                  #
################################################################################
LEAKS_LOG              = ./leaks.log
READLINE_SUPP          = readline.supp
VALGRINDFLAGS          = --tool=memcheck -q --leak-check=full \
                         --show-leak-kinds=all --track-origins=yes \
                         --track-fds=yes --show-below-main=no \



################################################################################
#                                 PROGRAM'S SRCS                               #
################################################################################

vpath %.h includes
vpath %.c src
MAIN = src/main.cpp
SRCS = $(shell find src -name '*.cpp' | sed 's/^/.\//')
INCLUDES = $(INCLUDE_HEADERS)

OBJ_DIR = obj
OBJS = $(patsubst %.c, $(OBJ_DIR)/%.o, $(SRCS))

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(EFLAGS) -D DEBUG=$(DEBUG) -c $< -o $@


################################################################################
#                                  Makefile  rules                             #
################################################################################

.PHONY: all
all: $(NAME)

.PHONY: mandatory
mandatory: $(NAME)

$(NAME): $(OBJS) --header
	@printf "$(GREEN)Compilation $(CLR_RMV)of $(YELLOW)libft$(CLR_RMV)...\n"
	@printf "$(GREEN)Compilation $(CLR_RMV)of $(YELLOW)$(NAME) $(CLR_RMV)...\n"
	@$(CC) $(CFLAGS) $(EFLAGS) $(OBJS) $(INCLUDES) -o $(NAME)
	@printf "$(GREEN)$(NAME) binary created$(CLR_RMV) ✅\n"



.PHONY: va
va : fclean $(OBJS) $(HEADER)
	@printf "$(GREEN)Compilation $(CLR_RMV)of $(YELLOW)$(NAME) $(CLR_RMV)...\n"
	@$(CC) $(CFLAGS) $(EFLAGS) $(OBJS) $(INCLUDES) -o $(NAME) 
	@echo -n valgrind $(VALGRINDFLAGS) ./$(NAME) scenes/ ; read args; valgrind $(VALGRINDFLAGS) ./$(NAME) scenes/$$args

.PHONY: clean
clean:
	@ $(RM) -f $(OBJS)
	@ printf "$(RED)Deleting $(CYAN)$(NAME) $(CLR_RMV)objs ✅\n"

.PHONY: fclean
fclean: clean
	@ $(RM) $(OBJ_DIR) $(NAME)
	@printf "$(RED)Deleting $(CYAN)$(NAME) $(CLR_RMV)binary ✅\n"

.PHONY: re
re : fclean all

.PHONY: header

--header :
	@ echo "$(RED)░▒▓██████████████▓▒░  $(BLUE)░▒▓█▓▒░ ░▒▓███████▓▒░   ░▒▓██████▓▒░  $(CLR_RMV)"
	@ echo "$(RED)░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░ $(BLUE)░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░ $(CLR_RMV)"
	@ echo "$(RED)░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░ $(BLUE)░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░        $(CLR_RMV)"
	@ echo "$(RED)░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░ $(BLUE)░▒▓█▓▒░ ░▒▓███████▓▒░  ░▒▓█▓▒░        $(CLR_RMV)"
	@ echo "$(RED)░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░ $(BLUE)░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░        $(CLR_RMV)"
	@ echo "$(RED)░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░ $(BLUE)░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░ $(CLR_RMV)"
	@ echo "$(RED)░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░ $(BLUE)░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░  ░▒▓██████▓▒░  $(CLR_RMV)"
	@ echo "             $(YELLOW)(developed by hluiz-ma && isilva-t && joao-pol)$(CLR_RMV)"
.SILENT: re all clean fclean
