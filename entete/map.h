#ifndef MAP_H
#define MAP_H

#include "types.h"
#include "settings.h"
#include <SDL2/SDL.h>

struct Map {
    int mini_map[MAP_ROWS][MAP_COLS];
    int rows;
    int cols;
};

void map_init(Map *map);
int  map_get(const Map *map, int x, int y);   /* texture id, 0 = empty */
int  map_is_wall(const Map *map, int x, int y);
void map_draw(const Map *map, SDL_Renderer *renderer);

#endif /* MAP_H */
