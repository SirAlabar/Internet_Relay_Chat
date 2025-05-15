################################################################################
#                                     CONFIG                                   #
################################################################################

NAME = ircserv
CC = c++
INCLUDE_HEADERS = -Iinclude -Iinclude/core -Iinclude/commands -Iinclude/bot -Iinclude/dcc -Iinclude/utils
CFLAGS = -Wall -Wextra -Werror -std=c++98 -Wno-delete-incomplete -Wno-unused-parameter
DEBUG_FLAGS = -g
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

# Main source files
SRC_DIR = src
SRCS = $(SRC_DIR)/main.cpp \
       $(SRC_DIR)/core/Server.cpp \
       $(SRC_DIR)/core/Client.cpp \
       $(SRC_DIR)/core/Message.cpp \
       $(SRC_DIR)/utils/Socket.cpp \
       $(SRC_DIR)/core/Channel.cpp

# Object files
OBJ_DIR = obj
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

################################################################################
#                                  Makefile  rules                             #
################################################################################

.PHONY: all
all: $(NAME)

$(NAME): --header $(OBJ_DIR) $(OBJS)
	@printf "$(GREEN)Compilation $(CLR_RMV)of $(YELLOW)$(NAME) $(CLR_RMV)...\n"
	@$(CC) $(CFLAGS) $(OBJS) $(INCLUDE_HEADERS) -o $(NAME)
	@printf "$(GREEN)$(NAME) binary created$(CLR_RMV) ✅\n"

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)/core
	@mkdir -p $(OBJ_DIR)/commands
	@mkdir -p $(OBJ_DIR)/utils
	@mkdir -p $(OBJ_DIR)/bot
	@mkdir -p $(OBJ_DIR)/dcc

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(INCLUDE_HEADERS) -c $< -o $@

.PHONY: va
va: fclean $(OBJS) 
	@printf "$(GREEN)Compilation $(CLR_RMV)of $(YELLOW)$(NAME) $(CLR_RMV)...\n"
	@$(CC) $(CFLAGS) $(OBJS) $(INCLUDE_HEADERS) -o $(NAME) 
	@echo -n "valgrind $(VALGRINDFLAGS) ./$(NAME) " ; read args; valgrind $(VALGRINDFLAGS) ./$(NAME) $$args

.PHONY: clean
clean:
	@$(RM) $(OBJ_DIR)
	@printf "$(RED)Deleting $(CYAN)$(NAME) $(CLR_RMV)objs ✅\n"

.PHONY: fclean
fclean: clean
	@$(RM) $(NAME)
	@printf "$(RED)Deleting $(CYAN)$(NAME) $(CLR_RMV)binary ✅\n"

.PHONY: re
re: fclean all

.PHONY: debug
debug: CFLAGS += $(DEBUG_FLAGS)
debug: re

.PHONY: --header
--header:
	@printf "$(RED)░▒▓██████████████▓▒░  $(BLUE)░▒▓█▓▒░ ░▒▓███████▓▒░   ░▒▓██████▓▒░  $(CLR_RMV)\n"
	@printf "$(RED)░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░ $(BLUE)░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░ $(CLR_RMV)\n"
	@printf "$(RED)░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░ $(BLUE)░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░        $(CLR_RMV)\n"
	@printf "$(RED)░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░ $(BLUE)░▒▓█▓▒░ ░▒▓███████▓▒░  ░▒▓█▓▒░        $(CLR_RMV)\n"
	@printf "$(RED)░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░ $(BLUE)░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░        $(CLR_RMV)\n"
	@printf "$(RED)░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░ $(BLUE)░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░ $(CLR_RMV)\n"
	@printf "$(RED)░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░ $(BLUE)░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░  ░▒▓██████▓▒░  $(CLR_RMV)\n"
	@printf "             $(YELLOW)(developed by hluiz, isilva-t & joao-pol)$(CLR_RMV)\n"