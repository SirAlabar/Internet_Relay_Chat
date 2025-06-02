################################################################################
#                                     CONFIG                                   #
################################################################################

NAME = ircserv
BOTNAME = ircbot
CC = c++
INCLUDE_HEADERS = -Iinclude -Iinclude/core -Iinclude/commands -Iinclude/bot -Iinclude/bot/commands_bot \
				  -Iinclude/dcc -Iinclude/utils -Iinclude/commands/connection \
				  -Iinclude/commands/messaging -Iinclude/commands/channel \
				  -Iinclude/commands/dcc
CFLAGS = -Wall -Wextra -Werror -std=c++98
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
		$(SRC_DIR)/commands/messaging/MotdCommand.cpp \
		$(SRC_DIR)/commands/channel/PrintdataCommand.cpp \
		$(SRC_DIR)/utils/Config.cpp

SRCSBOT =  $(SRC_DIR)/bot/Bot.cpp \
		$(SRC_DIR)/core/Server.cpp \
		$(SRC_DIR)/core/Client.cpp \
		$(SRC_DIR)/core/Message.cpp \
		$(SRC_DIR)/core/Channel.cpp \
		$(SRC_DIR)/bot/BotContext.cpp \
		$(SRC_DIR)/bot/ABotCommand.cpp \
		$(SRC_DIR)/bot/CommandBotFactory.cpp \
		$(SRC_DIR)/bot/commands_bot/DadJokesCommand.cpp \
		$(SRC_DIR)/bot/commands_bot/WeatherCommand.cpp \
		$(SRC_DIR)/bot/commands_bot/HelpCommand.cpp \
		$(SRC_DIR)/bot/commands_bot/GameCommand.cpp \
		$(SRC_DIR)/utils/Socket.cpp \
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
		$(SRC_DIR)/commands/messaging/MotdCommand.cpp \
		$(SRC_DIR)/commands/channel/PrintdataCommand.cpp \
		$(SRC_DIR)/utils/UtilsFun.cpp \
		$(SRC_DIR)/utils/HTTPClient.cpp \
		$(SRC_DIR)/utils/Config.cpp

# Bot-specific sources (calculate difference automatically)
SRCSBOT_UNIQUE = $(filter-out $(SRCS),$(SRCSBOT))

# Object files
OBJ_DIR = obj
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
OBJSBOT = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCSBOT))
OBJSBOT_UNIQUE = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o.bot,$(SRCSBOT_UNIQUE))
OBJS_NO_MAIN = $(filter-out $(OBJ_DIR)/main.o,$(OBJS))
OBJSBOT_REAL = $(OBJS_NO_MAIN) $(OBJSBOT_UNIQUE)

################################################################################
#                              PROGRESS BAR SETUP                              #
################################################################################

# Progress bar variables
TOTAL_FILES := $(words $(SRCS))
TOTAL_BOT_FILES := $(words $(SRCSBOT))
TOTAL_BOT_UNIQUE_FILES = $(words $(SRCSBOT_UNIQUE))
PROGRESS_FILE := .compilation_progress
COUNTER_FILE := .ircserv_compilation_counter
BOT_COUNTER_FILE := .ircbot_compilation_counter

# Progress bar function for main server
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
		printf "\n$(GREEN)✅ Server compilation complete!$(CLR_RMV)\n"; \
		rm -f $(COUNTER_FILE); \
	fi
endef

# Progress bar function for bot (only unique files)
define progress_bar_bot
	@current=$$(cat $(BOT_COUNTER_FILE) 2>/dev/null || echo 0); \
	current=$$((current + 1)); \
	echo $$current > $(BOT_COUNTER_FILE); \
	total_files=$(TOTAL_BOT_UNIQUE_FILES); \
	percent=$$((current * 100 / total_files)); \
	filled=$$((current * 50 / total_files)); \
	empty=$$((50 - filled)); \
	printf "\r\033[K$(CYAN)["; \
	for i in $$(seq 1 $$filled); do printf "█"; done; \
	for i in $$(seq 1 $$empty); do printf "░"; done; \
	printf "] %d%% (%d/$$total_files) $(YELLOW)%s$(CLR_RMV)" $$percent $$current "$(notdir $<)"; \
	if [ $$current -eq $$total_files ]; then \
		printf "\n$(GREEN)✅ Bot compilation complete!$(CLR_RMV)\n"; \
		rm -f $(BOT_COUNTER_FILE); \
	fi
endef

################################################################################
#                                  Makefile  rules                             #
################################################################################

.PHONY: all
all: $(NAME)

$(NAME): $(OBJ_DIR) $(OBJS)
	@printf "$(GREEN)Linking $(CLR_RMV)$(YELLOW)$(NAME)$(CLR_RMV)...\n"
	@$(CC) $(CFLAGS) $(OBJS) $(INCLUDE_HEADERS) $(DEBUG_DEFINE) -o $(NAME)
	@printf "$(GREEN)$(NAME) binary created$(CLR_RMV) ✅\n"

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)/core
	@mkdir -p $(OBJ_DIR)/commands
	@mkdir -p $(OBJ_DIR)/utils
	@mkdir -p $(OBJ_DIR)/bot
	@mkdir -p $(OBJ_DIR)/dcc

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@if [ ! -f .compilation_started ]; then \
		printf "$(RED)░▒▓██████████████▓▒░  $(BLUE)░▒▓█▓▒░ ░▒▓███████▓▒░   ░▒▓██████▓▒░  $(CLR_RMV)\n"; \
		printf "$(RED)░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░ $(BLUE)░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░ $(CLR_RMV)\n"; \
		printf "$(RED)░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░ $(BLUE)░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░        $(CLR_RMV)\n"; \
		printf "$(RED)░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░ $(BLUE)░▒▓█▓▒░ ░▒▓███████▓▒░  ░▒▓█▓▒░        $(CLR_RMV)\n"; \
		printf "$(RED)░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░ $(BLUE)░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░        $(CLR_RMV)\n"; \
		printf "$(RED)░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░ $(BLUE)░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░ $(CLR_RMV)\n"; \
		printf "$(RED)░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░ $(BLUE)░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░  ░▒▓██████▓▒░  $(CLR_RMV)\n"; \
		printf "             $(YELLOW)(developed by hluiz, isilva-t & joao-pol)$(CLR_RMV)\n"; \
		echo "0" > $(COUNTER_FILE); \
		printf "$(BLUE)Compiling $(TOTAL_FILES) source files for $(NAME)...$(CLR_RMV)\n"; \
		touch .compilation_started; \
	fi
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(INCLUDE_HEADERS) $(DEBUG_DEFINE) -c $< -o $@
	$(call progress_bar)

# Regra especial para objetos únicos do bot
$(OBJSBOT_UNIQUE): $(OBJ_DIR)/%.o.bot: $(SRC_DIR)/%.cpp
	@if [ ! -f .bot_compilation_started ]; then \
		printf "$(GREEN)░▒▓██████▓▒░   ░▒▓██████▓▒░  ░▒▓████████▓▒░$(CLR_RMV)\n"; \
		printf "$(GREEN)░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░   ░▒▓█▓▒░   $(CLR_RMV)\n"; \
		printf "$(GREEN)░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░   ░▒▓█▓▒░   $(CLR_RMV)\n"; \
		printf "$(GREEN)░▒▓███████▓▒░  ░▒▓█▓▒░░▒▓█▓▒░   ░▒▓█▓▒░   $(CLR_RMV)\n"; \
		printf "$(GREEN)░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░   ░▒▓█▓▒░   $(CLR_RMV)\n"; \
		printf "$(GREEN)░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░   ░▒▓█▓▒░   $(CLR_RMV)\n"; \
		printf "$(GREEN)░▒▓███████▓▒░   ░▒▓██████▓▒░    ░▒▓█▓▒░   $(CLR_RMV)\n"; \
		printf "             $(YELLOW)(IRC Bot by the same awesome team)$(CLR_RMV)\n"; \
		echo "0" > $(BOT_COUNTER_FILE); \
		printf "$(BLUE)Compiling $(TOTAL_BOT_UNIQUE_FILES) bot-specific files for $(BOTNAME)...$(CLR_RMV)\n"; \
		touch .bot_compilation_started; \
	fi
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(INCLUDE_HEADERS) $(DEBUG_DEFINE) -c $< -o $@
	$(call progress_bar_bot)

.PHONY: va
va: fclean $(OBJS)
	@printf "$(GREEN)Compilation $(CLR_RMV)of $(YELLOW)$(NAME) $(CLR_RMV)...\n"
	@$(CC) $(CFLAGS) $(OBJS) $(INCLUDE_HEADERS) -o $(NAME) 
	@echo -n "valgrind $(VALGRINDFLAGS) ./$(NAME) " ; read args; valgrind $(VALGRINDFLAGS) ./$(NAME) $$args

.PHONY: clean
clean:
	@if [ -d $(OBJ_DIR) ] || [ -f $(COUNTER_FILE) ] || [ -f $(BOT_COUNTER_FILE) ] || [ -f .compilation_started ] || [ -f .bot_compilation_started ]; then \
		$(RM) $(OBJ_DIR); \
		$(RM) $(COUNTER_FILE) $(BOT_COUNTER_FILE) $(PROGRESS_FILE) .compilation_started .bot_compilation_started; \
		printf "$(RED)Deleting $(CYAN)$(NAME) $(CLR_RMV)objs ✅\n"; \
	else \
		printf "$(YELLOW)Nothing to clean$(CLR_RMV) 🧹\n"; \
	fi

.PHONY: fclean
fclean: clean
	@if [ -f $(NAME) ] || [ -f $(BOTNAME) ]; then \
		$(RM) $(NAME) $(BOTNAME); \
		printf "$(RED)Deleting $(CYAN)$(NAME) $(CLR_RMV)binary ✅\n"; \
	else \
		printf "$(YELLOW)Nothing to delete$(CLR_RMV) 🗑️\n"; \
	fi

.PHONY: re
re: fclean all

.PHONY: bonus
bonus: $(NAME) $(BOTNAME)

$(BOTNAME): $(OBJ_DIR) $(OBJS) $(OBJSBOT_UNIQUE)
	@printf "$(GREEN)Linking $(CLR_RMV)$(YELLOW)$(BOTNAME)$(CLR_RMV)...\n"
	@$(CC) $(CFLAGS) $(OBJSBOT_REAL) $(INCLUDE_HEADERS) $(DEBUG_DEFINE) -o $(BOTNAME)
	@printf "$(GREEN)$(BOTNAME) binary created$(CLR_RMV) ✅\n"

.PHONY: debug
debug: CFLAGS += $(DEBUG_FLAGS)
debug: re


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
