UNAME_S := $(shell uname -s)

CC := gcc
CFLAGS := -Wall -Wextra -Wno-deprecated -std=c11

SRC := main.c collision.c GameObject.c GameEntity.c \
       physics/force.c physics/physics.c \
       objects/house.c objects/player.c objects/tree.c objects/object_primitives.c \
       utils/list.c utils/linked_list.c utils/queue.c utils/stack.c
TARGET := ./obj/main

ifeq ($(UNAME_S), Darwin)
    LDFLAGS := -framework GLUT -framework OpenGL -lm
else
    LDFLAGS := -lglut -lGL -lGLU -lm
endif

all: dir build

dir:
	mkdir -p obj bin

build: dir
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

run: build
	$(TARGET)

clean:
	rm -rf obj bin

.PHONY: all dir build run clean
