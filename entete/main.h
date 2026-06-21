#ifndef MAIN_H
#define MAIN_H

#include "settings.h"
#include "types.h"
#include "map.h"
#include "player.h"
#include "raycasting.h"
#include "object_renderer.h"
#include "sprite_object.h"
#include "npc.h"
#include "pathfinding.h"
#include "weapon.h"
#include "sound.h"
#include "object_handler.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>

/* The central game state — all subsystems embedded by value */
struct Game {
    SDL_Window   *window;
    SDL_Renderer *renderer;
    int           running;
    float         delta_time;
    int           global_trigger;

    Map            map;
    Player         player;
    ObjectRenderer object_renderer;
    RayCasting     raycasting;
    ObjectHandler  object_handler;
    Weapon         weapon;
    Sound          sound;
    PathFinding    pathfinding;
};

int  game_init(Game *game);
void game_new_game(Game *game);
void game_run(Game *game);
void game_update(Game *game);
void game_draw(Game *game);
void game_check_events(Game *game);
void game_cleanup(Game *game);

#endif /* MAIN_H */
