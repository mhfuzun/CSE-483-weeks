CC ?= clang
TARGET := render-pipeline
BUILD_DIR := build

SRC := main.c backend/backend_sdl.c backend/ptime.c \
       lectures/0_render.c \
       lectures/1_483lec1line24s.c \
       lectures/4_483lec4trans3D26s.c \
       lectures/5_483lec6b3Dviewing21s.c
OBJ := $(addprefix $(BUILD_DIR)/,$(SRC:.c=.o))

SDL2_CONFIG ?= $(firstword $(shell command -v sdl2-config 2>/dev/null) $(wildcard /opt/homebrew/bin/sdl2-config) $(wildcard /usr/local/bin/sdl2-config))
PKG_CONFIG ?= $(firstword $(shell command -v pkg-config 2>/dev/null) $(wildcard /opt/homebrew/bin/pkg-config) $(wildcard /usr/local/bin/pkg-config))

SDL_CFLAGS ?= $(shell $(SDL2_CONFIG) --cflags 2>/dev/null)
SDL_LIBS ?= $(shell $(SDL2_CONFIG) --libs 2>/dev/null)
SDL_TTF_CFLAGS ?= $(shell $(PKG_CONFIG) --cflags SDL2_ttf 2>/dev/null)
SDL_TTF_LIBS ?= $(shell $(PKG_CONFIG) --libs SDL2_ttf 2>/dev/null)

ifeq ($(strip $(SDL_CFLAGS)),)
SDL_CFLAGS := $(shell $(PKG_CONFIG) --cflags sdl2 2>/dev/null)
SDL_LIBS := $(shell $(PKG_CONFIG) --libs sdl2 2>/dev/null)
endif

ifeq ($(strip $(SDL_CFLAGS)),)
$(error SDL2 bulunamadi. macOS icin: brew install sdl2 pkg-config)
endif

ifeq ($(strip $(SDL_TTF_LIBS)),)
$(error SDL2_ttf bulunamadi. macOS icin: brew install sdl2_ttf pkg-config)
endif

SDL_TTF_LIBS := $(filter-out -lSDL2,$(SDL_TTF_LIBS))

CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -O2
CPPFLAGS += -Iinclude -I. -Itypes $(SDL_CFLAGS) $(SDL_TTF_CFLAGS)
LDLIBS += $(SDL_LIBS) $(SDL_TTF_LIBS) -lm

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) $(LDLIBS) -o $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)
