
CC			:= gcc

MAIN_FILE	:= main

SRC_DIR		:= ./src
OBJ_DIR		:= ./obj
BIN_DIR		:= ./bin

CC_FLAGS	:= -lglut -lGL -lGLU

all: dir build

dir:
	mkdir -p $(SRC_DIR) $(OBJ_DIR) $(BIN_DIR)

build: dir
	$(CC) $(SRC_DIR)/$(MAIN_FILE).c -o $(OBJ_DIR)/$(MAIN_FILE) $(CC_FLAGS)

run:
	$(OBJ_DIR)/$(MAIN_FILE)
