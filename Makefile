# Variables
PROJECT	:= web-server 

SRC_DIR	:= ./src
INC_DIR	:= ./include
OBJ_DIR	:= ${BUILD_DIR}/obj
BUILD_DIR	:= ./build

CC	:= gcc
CC_CFLAGS	:= -I${INC_DIR} -Wall -Wextra -Wpedantic -Werror -Wshadow -Wformat=2 -Wconversion -Wunused-parameter -std=c11 -g

EXE	:= $(BUILD_DIR)/web-server
SRC	:= $(wildcard $(SRC_DIR)/*.c)
OBJ	:= $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

#Rules
.PHONY: all clean

all: $(EXE) 

$(EXE): $(BUILD_DIR)
	$(CC) $(CC_CFLAGS) -o $@ ${SRC}
		
$(BUILD_DIR) $(OBJ_DIR): 
	mkdir -p $@

clean:
	@$(RM) -rv $(BUILD_DIR) $(OBJ_DIR)
