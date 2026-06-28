# ---------------------------------------------------------------------------
# Cross-platform Makefile : Linux + Windows (MSYS2/MinGW, native or cross)
# ---------------------------------------------------------------------------
#
#   make            -> native build for the current OS
#   make windows    -> cross-compile a Windows .exe from Linux (MinGW-w64)
#   make clean
#
# Windows native build: run from an MSYS2 MinGW64 shell with:
#   pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-pkg-config \
#             mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image \
#             mingw-w64-x86_64-SDL2_mixer
# ---------------------------------------------------------------------------

SDL_PKGS = sdl2 SDL2_image SDL2_mixer

SRCS = main.c map.c player.c raycasting.c object_renderer.c \
       sprite_object.c npc.c pathfinding.c weapon.c sound.c \
       object_handler.c
OBJS = $(SRCS:.c=.o)

# --- Platform detection ----------------------------------------------------
ifeq ($(OS),Windows_NT)
    PLATFORM := windows
else
    PLATFORM := $(shell uname -s)
endif

CC          ?= gcc
PKG_CONFIG  ?= pkg-config
BASE_CFLAGS  = -std=c99 -Wall -Wextra -O2 -Ientete
LDLIBS_EXTRA = -lm

ifeq ($(PLATFORM),windows)
    TARGET = doom.exe
    LDLIBS_EXTRA += -lmingw32
else
    TARGET = doom
endif

CFLAGS  = $(BASE_CFLAGS) $(shell $(PKG_CONFIG) --cflags $(SDL_PKGS))
LDFLAGS = $(shell $(PKG_CONFIG) --libs $(SDL_PKGS)) $(LDLIBS_EXTRA)

.PHONY: all clean windows

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# --- compile Windows binary from Linux -----------------------------
MINGW_PREFIX ?= x86_64-w64-mingw32
windows:
	$(MAKE) clean
	$(MAKE) all \
	    CC=$(MINGW_PREFIX)-gcc \
	    PKG_CONFIG="$(MINGW_PREFIX)-pkg-config" \
	    OS=Windows_NT

clean:
	rm -f $(OBJS) doom doom.exe
