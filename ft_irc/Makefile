################################################################################
#                                     CONFIG                                   #
################################################################################

NAME = ircserv
CC = c++
INCLUDE_HEADERS = -Iinclude -Iinclude/core -Iinclude/commands -Iinclude/bot \
				  -Iinclude/dcc -Iinclude/utils -Iinclude/commands/connection \
				  -Iinclude/commands/messaging -Iinclude/commands/channel \
				  -Iinclude/commands/dcc
CFLAGS = -Wall -Wextra -Werror -std=c++98 -Wno-delete-incomplete -Wno-unused-parameter
ASANFLAGS = -fsanitize=undefined,memory -fno-omit-frame-pointer
DEBUG_FLAGS = -g

ifeq ($(DEBUG), 1)
	CFLAGS += $(DEBUG_FLAGS)
	DEBUG_DEFINE = -DDEBUG=1
else
	DEBUG_DEFINE = -DDEBUG=0
endif


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
SRCS =  $(SRC_DIR)/main.cpp \
		$(SRC_DIR)/core/Server.cpp \
		$(SRC_DIR)/core/Client.cpp \
		$(SRC_DIR)/core/Message.cpp \
		$(SRC_DIR)/utils/Socket.cpp \
		$(SRC_DIR)/core/Channel.cpp \
		$(SRC_DIR)/utils/UtilsFun.cpp \
		$(SRC_DIR)/commands/CommandFactory.cpp \
		$(SRC_DIR)/commands/ACommand.cpp \
		$(SRC_DIR)/commands/channel/JoinCommand.cpp \
		$(SRC_DIR)/commands/channel/ListCommand.cpp \
		$(SRC_DIR)/commands/connection/NickCommand.cpp \
		$(SRC_DIR)/commands/connection/PassCommand.cpp \
		$(SRC_DIR)/commands/connection/UserCommand.cpp \
		$(SRC_DIR)/commands/connection/CapCommand.cpp \
		$(SRC_DIR)/commands/connection/PingCommand.cpp \
		$(SRC_DIR)/commands/connection/PongCommand.cpp \
		$(SRC_DIR)/commands/connection/QuitCommand.cpp \
		$(SRC_DIR)/commands/messaging/NoticeCommand.cpp \
		$(SRC_DIR)/commands/messaging/WhoCommand.cpp \
		$(SRC_DIR)/commands/messaging/WhoIsCommand.cpp 

# Object files
OBJ_DIR = obj
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

################################################################################
#                              PROGRESS BAR SETUP                              #
################################################################################

# Progress bar variables
TOTAL_FILES := $(words $(SRCS))
PROGRESS_FILE := .compilation_progress
COUNTER_FILE := /tmp/.ircserv_compilation_counter

# Progress bar function
define progress_bar
	@current=$$(cat $(COUNTER_FILE) 2>/dev/null || echo 0); \
	current=$$((current + 1)); \
	echo $$current > $(COUNTER_FILE); \
	percent=$$((current * 100 / $(TOTAL_FILES))); \
	filled=$$((current * 50 / $(TOTAL_FILES))); \
	empty=$$((50 - filled)); \
	printf "\r\033[K$(CYAN)["; \
	for i in $$(seq 1 $$filled); do printf "█"; done; \
	for i in $$(seq 1 $$empty); do printf "░"; done; \
	printf "] %d%% (%d/$(TOTAL_FILES)) $(YELLOW)%s$(CLR_RMV)" $$percent $$current "$(notdir $<)"; \
	if [ $$current -eq $(TOTAL_FILES) ]; then \
		printf "\n$(GREEN)✅ Compilation complete!$(CLR_RMV)\n"; \
		rm -f $(COUNTER_FILE) init_progress; \
	fi
endef

################################################################################
#                                  Makefile  rules                             #
################################################################################

.PHONY: all
all: $(NAME)

$(NAME): $(OBJ_DIR) init_progress $(OBJS)
	@printf "$(GREEN)Compilation $(CLR_RMV)of $(YELLOW)$(NAME) $(CLR_RMV)...\n"
	@$(CC) $(CFLAGS) $(OBJS) $(INCLUDE_HEADERS) $(DEBUG_DEFINE) -o $(NAME)

	@printf "$(GREEN)$(NAME) binary created$(CLR_RMV) ✅\n"

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)/core
	@mkdir -p $(OBJ_DIR)/commands
	@mkdir -p $(OBJ_DIR)/utils
	@mkdir -p $(OBJ_DIR)/bot
	@mkdir -p $(OBJ_DIR)/dcc

init_progress: $(SRCS)
	@printf "$(RED)░▒▓██████████████▓▒░  $(BLUE)░▒▓█▓▒░ ░▒▓███████▓▒░   ░▒▓██████▓▒░  $(CLR_RMV)\n"
	@printf "$(RED)░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░ $(BLUE)░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░ $(CLR_RMV)\n"
	@printf "$(RED)░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░ $(BLUE)░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░        $(CLR_RMV)\n"
	@printf "$(RED)░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░ $(BLUE)░▒▓█▓▒░ ░▒▓███████▓▒░  ░▒▓█▓▒░        $(CLR_RMV)\n"
	@printf "$(RED)░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░ $(BLUE)░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░        $(CLR_RMV)\n"
	@printf "$(RED)░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░ $(BLUE)░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░ $(CLR_RMV)\n"
	@printf "$(RED)░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░ $(BLUE)░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░  ░▒▓██████▓▒░  $(CLR_RMV)\n"
	@printf "             $(YELLOW)(developed by hluiz, isilva-t & joao-pol)$(CLR_RMV)\n"
	@echo "0" > $(COUNTER_FILE)
	@printf "$(BLUE)Compiling $(TOTAL_FILES) source files...$(CLR_RMV)\n"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)

	@$(CC) $(CFLAGS) $(INCLUDE_HEADERS) $(DEBUG_DEFINE) -c $< -o $@
	$(call progress_bar)

.PHONY: va
va: fclean $(OBJS) 
	@printf "$(GREEN)Compilation $(CLR_RMV)of $(YELLOW)$(NAME) $(CLR_RMV)...\n"
	@$(CC) $(CFLAGS) $(OBJS) $(INCLUDE_HEADERS) -o $(NAME) 
	@echo -n "valgrind $(VALGRINDFLAGS) ./$(NAME) " ; read args; valgrind $(VALGRINDFLAGS) ./$(NAME) $$args

.PHONY: clean
clean:
	@$(RM) $(OBJ_DIR)
	@$(RM) $(COUNTER_FILE) $(PROGRESS_FILE) init_progress
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

.PHONY: asan
asan: --header $(OBJ_DIR) $(OBJS)

	@printf "$(GREEN)Compilation $(CLR_RMV)of $(YELLOW)$(NAME) $(CLR_RMV)...\n"
	@$(CC) $(CFLAGS) $(OBJS) $(INCLUDE_HEADERS) $(DEBUG_DEFINE) -o $(NAME)

	@printf "$(GREEN)$(NAME) binary created$(CLR_RMV) ✅\n"


################################################################################
#                              Makefile  other utils                           #
################################################################################

.PHONY: out
out:
	touch output_file.txt
	rm output_file.txt
	find . -type f \( -name "*.cpp" -o -name "*.hpp" \) -not -path "*/\.*" -not -path "*/obj/*" -not -path "*/tests/*" | while read file; do \
		relative_path=$$(echo $$file | sed "s|^\./||"); \
		echo "### Starting file: $$relative_path" >> output_file.txt; \
		echo "" >> output_file.txt; \
		cat "$$file" >> output_file.txt; \
		echo "" >> output_file.txt; \
		echo "" >> output_file.txt; \
	done

.PHONY: vi
vi:
	@find . -type f -name "*.cpp" -o -name "*.hpp" -o -name "Makefile" | xargs nvim

