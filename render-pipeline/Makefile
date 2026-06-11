CC ?= clang
TARGET := render-pipeline
BUILD_DIR := build

UNAME_S := $(shell uname -s 2>/dev/null)
ifeq ($(UNAME_S),Darwin)
DEPS_HINT := macOS icin: brew install sdl2 sdl2_ttf pkg-config
else ifeq ($(UNAME_S),Linux)
DEPS_HINT := Linux Mint icin: sudo apt install build-essential pkg-config libsdl2-dev libsdl2-ttf-dev
else
DEPS_HINT := SDL2 ve SDL2_ttf gelistirme paketlerini kurun.
endif

SRC := main.c backend/backend_sdl.c backend/ptime.c \
       types/mesh.c \
       lectures/0_render.c \
       lectures/1_483lec1line24s.c \
       lectures/4_483lec4trans3D26s.c \
       lectures/5_483lec6b3Dviewing21s.c \
       lectures/9_483lec8light23s.c
OBJ := $(addprefix $(BUILD_DIR)/,$(SRC:.c=.o))

SDL2_CONFIG ?= $(firstword $(shell command -v sdl2-config 2>/dev/null) $(wildcard /opt/homebrew/bin/sdl2-config) $(wildcard /usr/local/bin/sdl2-config))
PKG_CONFIG ?= $(firstword $(shell command -v pkg-config 2>/dev/null) $(wildcard /opt/homebrew/bin/pkg-config) $(wildcard /usr/local/bin/pkg-config))

SDL_CFLAGS ?= $(if $(SDL2_CONFIG),$(shell $(SDL2_CONFIG) --cflags 2>/dev/null))
SDL_LIBS ?= $(if $(SDL2_CONFIG),$(shell $(SDL2_CONFIG) --libs 2>/dev/null))
SDL_TTF_CFLAGS ?= $(if $(PKG_CONFIG),$(shell $(PKG_CONFIG) --cflags SDL2_ttf 2>/dev/null))
SDL_TTF_LIBS ?= $(if $(PKG_CONFIG),$(shell $(PKG_CONFIG) --libs SDL2_ttf 2>/dev/null))
PNG_CFLAGS ?= $(if $(PKG_CONFIG),$(shell $(PKG_CONFIG) --cflags libpng 2>/dev/null))
PNG_LIBS ?= $(if $(PKG_CONFIG),$(shell $(PKG_CONFIG) --libs libpng 2>/dev/null))

ifeq ($(strip $(SDL_CFLAGS)),)
ifneq ($(strip $(PKG_CONFIG)),)
SDL_CFLAGS := $(shell $(PKG_CONFIG) --cflags sdl2 2>/dev/null)
SDL_LIBS := $(shell $(PKG_CONFIG) --libs sdl2 2>/dev/null)
endif
endif

ifeq ($(strip $(SDL_CFLAGS)),)
$(error SDL2 bulunamadi. $(DEPS_HINT))
endif

ifeq ($(strip $(SDL_TTF_LIBS)),)
$(error SDL2_ttf bulunamadi. $(DEPS_HINT))
endif

ifeq ($(strip $(PNG_LIBS)),)
$(error libpng bulunamadi. Linux icin: sudo apt install libpng-dev)
endif

SDL_TTF_LIBS := $(filter-out -lSDL2,$(SDL_TTF_LIBS))

CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -O3
CPPFLAGS += -Iinclude -I. -Itypes $(SDL_CFLAGS) $(SDL_TTF_CFLAGS) $(PNG_CFLAGS)
LDLIBS += $(SDL_LIBS) $(SDL_TTF_LIBS) $(PNG_LIBS) -lm

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
