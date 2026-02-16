<<<<<<< HEAD
# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rde-fari <rde-fari@student.42porto.com>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/01/12 17:54:45 by rde-fari          #+#    #+#              #
#    Updated: 2026/01/16 22:18:25 by rde-fari         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# ==============================================================================
# VARIABLES & PATHS
# VARIABLES & PATHS
# ==============================================================================
MAKEFLAGS	+= --silent
NAME        = ircserv
CXX         = c++
CXXFLAGS    = -Wall -Wextra -Werror -std=c++98 -g3 -fsanitize=address
SRC_DIR     = src
OBJ_DIR     = obj
INC_DIR     = include
INCLUDES    = -I $(INC_DIR)
SRCS        = $(shell find $(SRC_DIR) -name "*.cpp")
OBJS        = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))
=======
# Flags & Variables
MAKEFLAGS += -s
NAME = ircserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -I inc/ -g3 -fsanitize=address
SRC_DIR = src
OBJ_DIR = obj
INC_DIR = inc
SRC = $(shell find $(SRC_DIR) -name "*.cpp")
OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Progress + LOC variables
TOTAL_FILES := $(words $(OBJ))
COUNTER_FILE := $(OBJ_DIR)/.build_counter
LOC_FILES := $(shell find $(SRC_DIR) $(INC_DIR) -type f \( -name "*.cpp" -o -name "*.hpp" \))
>>>>>>> origin/feature/Parser&Dispatch

# Colors & Text Formatting
RED				= \033[0;31m
GREEN			= \033[0;32m
YELLOW			= \033[0;33m
BLUE			= \033[0;34m
CYAN            = \033[0;36m
RESET			= \033[0m
BOLD			= \033[1m

# Progress bar
define progress_bar
	@current=$$(cat $(COUNTER_FILE) 2>/dev/null || echo 0); \
	current=$$((current + 1)); \
	echo $$current > $(COUNTER_FILE); \
	total=$(TOTAL_FILES); \
	percent=$$((current * 100 / total)); \
	filled=$$((current * 50 / total)); \
	empty=$$((50 - filled)); \
	printf "\r\033[K$(CYAN)["; \
	for i in $$(seq 1 $$filled 2>/dev/null); do printf "█"; done; \
	for i in $$(seq 1 $$empty 2>/dev/null); do printf "░"; done; \
	printf "] %d%% (%d/%d) $(YELLOW)%s$(RESET)" $$percent $$current $$total "$(notdir $<)"; \
	if [ $$current -ge $$total ]; then \
		printf "\n"; \
		rm -f $(COUNTER_FILE); \
	fi
endef

# Line counter
define count_lines
	@total=$$(wc -l $(LOC_FILES) | tail -n 1 | awk '{print $$1}'); \
	printf "$(BLUE)[ i ] Total lines of code: $(BOLD)%s$(RESET)\n" $$total
endef

all: $(NAME)
	printf "$(BOLD)$(GREEN)[ ✓ ] COMPILATION SUCCESSFUL!$(RESET)\n"

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)
	printf "$(BLUE)[ + ] Creating object directory$(RESET)\n"
	rm -f $(COUNTER_FILE)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@
	$(progress_bar)

$(NAME): $(OBJ)
	printf "$(BLUE)[ + ] Linking executable$(RESET)\n"
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)
	$(count_lines)

clean:
	printf "$(RED)[ x ] Cleaning object files$(RESET)\n"
	rm -rf $(OBJ_DIR)

fclean: clean
	printf "$(RED)[ x ] Full clean - removing executable$(RESET)\n"
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re