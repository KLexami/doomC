#ifndef PLAYER_H
#define PLAYER_H

#include "types.h"
#include "settings.h"
#include <SDL2/SDL.h>

struct Player {
    float   x, y;
    float   angle;
    int     shot;
    int     health;
    int     rel;            /* mouse relative movement */
    int     health_recovery_delay; /* ms */
    Uint32  time_prev;
    float   diag_move_corr; /* 1/sqrt(2) for diagonal movement */
};

void player_init(Player *p);
void player_update(Player *p, Game *game);
void player_movement(Player *p, Game *game);
void player_mouse_control(Player *p, Game *game);
void player_recover_health(Player *p);
void player_check_game_over(Player *p, Game *game);
void player_get_damage(Player *p, Game *game, int damage);
void player_single_fire_event(Player *p, Game *game, const SDL_Event *event);
int  player_check_wall(const Game *game, int x, int y);
void player_check_wall_collision(Player *p, const Game *game, float dx, float dy);

/* Inline helpers for map position */
static inline int player_map_x(const Player *p) { return (int)p->x; }
static inline int player_map_y(const Player *p) { return (int)p->y; }

#endif /* PLAYER_H */
