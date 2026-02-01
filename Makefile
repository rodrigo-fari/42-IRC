# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rerodrig <rerodrig@student.42porto.com>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/01/12 17:54:45 by rde-fari          #+#    #+#              #
#    Updated: 2026/01/30 18:22:41 by rerodrig         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# ==============================================================================
# VARIABLES & PATHS
# ==============================================================================
MAKEFLAGS	+= --silent
NAME        = ircserv
CXX         = c++
SANITIZERS  = -fsanitize=address
CXXFLAGS    = -Wall -Wextra -Werror -std=c++98 -g3
SRC_DIR     = src
OBJ_DIR     = obj
INC_DIR     = include
INCLUDES    = -I $(INC_DIR)
SRCS        = $(shell find $(SRC_DIR) -name "*.cpp")
OBJS        = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# ==============================================================================
# COLORS & FORMATTING
# ==============================================================================
RED         = \033[0;31m
GREEN       = \033[0;32m
YELLOW      = \033[0;33m
BLUE        = \033[0;34m
RESET       = \033[0m
BOLD        = \033[1m

# ==============================================================================
# RULES
# ==============================================================================

all: $(NAME)

$(NAME): $(OBJS)
	@printf "$(BLUE)[ + ] Linking executable: $(NAME)$(RESET)\n"
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@printf "$(GREEN)$(BOLD)[ ✓ ] COMPILATION SUCCESSFUL!$(RESET)\n"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@printf "$(YELLOW)[ + ] Compiling $(notdir $<)$(RESET)\n"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@printf "$(RED)[ x ] Cleaning object files$(RESET)\n"
	@rm -rf $(OBJ_DIR)

fclean: clean
	@printf "$(RED)[ x ] Full clean - removing executable$(RESET)\n"
	@rm -f $(NAME)

re: fclean all

rr: re
	@./$(NAME)

.PHONY: all clean fclean re rr