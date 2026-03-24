UNAME_S 	:= $(shell uname -s)

CC			:= gcc

MAIN_FILE	:= main

SRC_DIR		:= ./src
OBJ_DIR		:= ./obj
BIN_DIR		:= ./bin

CFLAGS		:= -Wall
CFLAGS		+= -Wno-deprecated

TARGET		:= $(OBJ_DIR)/$(MAIN_FILE)

ifeq ($(UNAME_S), Darwin)
    # Native OpenGL Framework
    LDFLAGS 	= -framework GLUT -framework OpenGL
else
    # Linux
    LDFLAGS 	= -lglut -lGL -lGLU
endif

all: dir build

dir:
	mkdir -p $(SRC_DIR) $(OBJ_DIR) $(BIN_DIR)

build: dir
	$(CC) $(CFLAGS) $(SRC_DIR)/$(MAIN_FILE).c -o $(OBJ_DIR)/$(MAIN_FILE) $(LDFLAGS)

run: $(TARGET)
	$(TARGET)
