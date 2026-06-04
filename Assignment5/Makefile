UNAME_S 	:= $(shell uname -s)

CC			:= gcc

MAIN_FILE	:= main

SRC_DIR		:= .
OBJ_DIR		:= ./obj
BIN_DIR		:= ./bin

CFLAGS		:= -Wall
CFLAGS		+= -Wno-deprecated

TARGET		:= $(OBJ_DIR)/$(MAIN_FILE)

.PHONY: all dir build run run_full start_vnc clean

ifeq ($(UNAME_S), Darwin)
    # Native OpenGL Framework
    LDFLAGS 	= -framework GLUT -framework OpenGL
else
    # Linux
    LDFLAGS 	= -lglut -lGL -lGLU -lm
endif

all: dir build

dir:
	mkdir -p $(OBJ_DIR) $(BIN_DIR)

build: dir
	$(CC) $(CFLAGS) $(SRC_DIR)/$(MAIN_FILE).c -o $(OBJ_DIR)/$(MAIN_FILE) $(LDFLAGS)

run: build
	$(TARGET)

run_full: build
	$(TARGET) --size 1080x2408 --fullscreen

start_vnc:
	x11vnc -display :0 -auth guess -forever -shared -rfbauth ~/.vnc/passwd -rfbport 5900 -geometry 1920x1080 &

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
