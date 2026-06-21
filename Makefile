CC      = gcc
CFLAGS  = -std=c99 -Wall -Wextra -O2 -Ientete
LDFLAGS = -lSDL2 -lSDL2_image -lSDL2_mixer -lm

SRCS = main.c map.c player.c raycasting.c object_renderer.c \
       sprite_object.c npc.c pathfinding.c weapon.c sound.c \
       object_handler.c

OBJS    = $(SRCS:.c=.o)
TARGET  = doom

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET).exe $(TARGET)

# --- Dependency hints (SDL2 paths may vary) ---
# With MSYS2/MinGW64 (SDL2 installed via pacman):
#   make CFLAGS="-std=c99 -Wall -O2 $(sdl2-config --cflags)" \
#        LDFLAGS="$(sdl2-config --libs) -lSDL2_image -lSDL2_mixer -lm -mwindows"
#
# Resources folder must be next to the executable:
#   cp -r ../resources .
