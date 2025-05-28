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
		$(SRC_DIR)/commands/channel/ModeCommand.cpp \
		$(SRC_DIR)/commands/channel/InviteCommand.cpp \
		$(SRC_DIR)/commands/channel/ListCommand.cpp \
		$(SRC_DIR)/commands/channel/PartCommand.cpp \
		$(SRC_DIR)/commands/channel/KickCommand.cpp \
		$(SRC_DIR)/commands/connection/NickCommand.cpp \
		$(SRC_DIR)/commands/connection/PassCommand.cpp \
		$(SRC_DIR)/commands/connection/UserCommand.cpp \
		$(SRC_DIR)/commands/connection/CapCommand.cpp \
		$(SRC_DIR)/commands/connection/PingCommand.cpp \
		$(SRC_DIR)/commands/connection/PongCommand.cpp \
		$(SRC_DIR)/commands/connection/QuitCommand.cpp \
		$(SRC_DIR)/commands/messaging/NoticeCommand.cpp \
		$(SRC_DIR)/commands/messaging/PrivmsgCommand.cpp \
		$(SRC_DIR)/commands/messaging/WhoCommand.cpp \
		$(SRC_DIR)/commands/messaging/WhoIsCommand.cpp \
		$(SRC_DIR)/commands/channel/TopicCommand.cpp \
		$(SRC_DIR)/commands/messaging/MotdCommand.cpp

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


################################################################################
#                                  TEST RULES                                  #
################################################################################

.PHONY: test test-connection test-channels test-messaging test-stress test-valgrind test-all test-setup test-clean

# Setup test environment
test-setup:
	@printf "$(BLUE)Setting up test environment...$(CLR_RMV)\n"
	@mkdir -p tests/scripts
	@chmod +x tests/scripts/*.sh 2>/dev/null || true
	@printf "$(GREEN)Test environment ready$(CLR_RMV) ✅\n"

# Run all tests (default)
test: $(NAME) test-setup
	@printf "$(GREEN)Running complete IRC test suite...$(CLR_RMV)\n"
	@./tests/scripts/run_tests.sh

# Run specific test suites
test-connection: $(NAME) test-setup
	@printf "$(BLUE)Running connection tests...$(CLR_RMV)\n"
	@./tests/scripts/test_connection.sh

test-channels: $(NAME) test-setup
	@printf "$(BLUE)Running channel tests...$(CLR_RMV)\n"
	@./tests/scripts/test_channels.sh

test-messaging: $(NAME) test-setup
	@printf "$(BLUE)Running messaging tests...$(CLR_RMV)\n"
	@./tests/scripts/test_messaging.sh

test-stress: $(NAME) test-setup
	@printf "$(YELLOW)Running stress tests (this may take a while)...$(CLR_RMV)\n"
	@./tests/scripts/test_stress.sh

test-valgrind: $(NAME) test-setup
	@printf "$(YELLOW)Running memory leak tests with Valgrind (this will take several minutes)...$(CLR_RMV)\n"
	@./tests/scripts/test_valgrind.sh

# Run all tests including stress and valgrind
test-all: $(NAME) test-setup
	@printf "$(GREEN)Running ALL tests (including stress and valgrind)...$(CLR_RMV)\n"
	@printf "$(YELLOW)This will take 10+ minutes. Press Ctrl+C to cancel.$(CLR_RMV)\n"
	@sleep 3
	@./tests/scripts/run_tests.sh && \
	 ./tests/scripts/test_connection.sh && \
	 ./tests/scripts/test_channels.sh && \
	 ./tests/scripts/test_messaging.sh && \
	 ./tests/scripts/test_stress.sh && \
	 ./tests/scripts/test_valgrind.sh
	@printf "$(GREEN)🎉 All tests completed successfully!$(CLR_RMV)\n"

# Quick test (basic functionality only)
test-quick: $(NAME) test-setup
	@printf "$(BLUE)Running quick tests...$(CLR_RMV)\n"
	@./tests/scripts/test_connection.sh

# Clean test artifacts
test-clean:
	@printf "$(RED)Cleaning test artifacts...$(CLR_RMV)\n"
	@rm -f tests/*.log
	@rm -f *.log
	@rm -f valgrind_output.log
	@printf "$(GREEN)Test cleanup completed$(CLR_RMV) ✅\n"

# Test with debug output
test-debug: CFLAGS += $(DEBUG_FLAGS) -DDEBUG=1
test-debug: re test

# Show test help
test-help:
	@printf "$(BLUE)IRC Test Suite Commands:$(CLR_RMV)\n"
	@printf "$(GREEN)make test$(CLR_RMV)           - Run basic test suite\n"
	@printf "$(GREEN)make test-quick$(CLR_RMV)     - Run only quick connection tests\n"
	@printf "$(GREEN)make test-connection$(CLR_RMV) - Test authentication and connection\n"
	@printf "$(GREEN)make test-channels$(CLR_RMV)   - Test channel operations (JOIN, PART, etc.)\n"
	@printf "$(GREEN)make test-messaging$(CLR_RMV)  - Test PRIVMSG functionality\n"
	@printf "$(GREEN)make test-stress$(CLR_RMV)     - Test server under load (slow)\n"
	@printf "$(GREEN)make test-valgrind$(CLR_RMV)   - Check for memory leaks (very slow)\n"
	@printf "$(GREEN)make test-all$(CLR_RMV)        - Run ALL tests (10+ minutes)\n"
	@printf "$(GREEN)make test-debug$(CLR_RMV)      - Run tests with debug build\n"
	@printf "$(GREEN)make test-clean$(CLR_RMV)      - Clean test artifacts\n"
	@printf "$(BLUE)Example usage:$(CLR_RMV)\n"
	@printf "  make && make test-quick    # Quick verification\n"
	@printf "  make test-channels         # Test channel commands\n"
	@printf "  make test-all             # Full test suite\n"
