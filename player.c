#include "main.h"
#include <math.h>
#include <stdio.h>

void player_init(Player *p) {
    p->x                     = PLAYER_POS_X;
    p->y                     = PLAYER_POS_Y;
    p->angle                 = PLAYER_ANGLE;
    p->shot                  = 0;
    p->health                = PLAYER_MAX_HEALTH;
    p->rel                   = 0;
    p->health_recovery_delay = 700;
    p->time_prev             = SDL_GetTicks();
    p->diag_move_corr        = 1.0f / (float)sqrt(2.0);
}

int player_check_wall(const Game *game, int x, int y) {
    return !map_is_wall(&game->map, x, y);
}

void player_check_wall_collision(Player *p, const Game *game, float dx, float dy) {
    float scale = (float)PLAYER_SIZE_SCALE / game->delta_time;
    if (player_check_wall(game, (int)(p->x + dx * scale), (int)p->y))
        p->x += dx;
    if (player_check_wall(game, (int)p->x, (int)(p->y + dy * scale)))
        p->y += dy;
}

void player_movement(Player *p, Game *game) {
    float sin_a = (float)sin(p->angle);
    float cos_a = (float)cos(p->angle);
    float dx = 0.0f, dy = 0.0f;
    float speed      = PLAYER_SPEED * game->delta_time;
    float speed_sin  = speed * sin_a;
    float speed_cos  = speed * cos_a;

    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    int num_keys = -1;

    if (keys[SDL_SCANCODE_W]) { num_keys++;  dx +=  speed_cos; dy +=  speed_sin; }
    if (keys[SDL_SCANCODE_S]) { num_keys++;  dx += -speed_cos; dy += -speed_sin; }
    if (keys[SDL_SCANCODE_A]) { num_keys++;  dx +=  speed_sin; dy += -speed_cos; }
    if (keys[SDL_SCANCODE_D]) { num_keys++; dx += -speed_sin; dy +=  speed_cos; }

    if (num_keys) {
        dx *= p->diag_move_corr;
        dy *= p->diag_move_corr;
    }

    player_check_wall_collision(p, game, dx, dy);

    /* Wrap angle to [0, 2π) */
    p->angle = (float)fmod(p->angle, 2.0 * M_PI);
    if (p->angle < 0.0f) p->angle += (float)(2.0 * M_PI);
}

void player_mouse_control(Player *p, Game *game) {
    int mx, my;
    SDL_GetMouseState(&mx, &my);

    if (mx < MOUSE_BORDER_LEFT || mx > MOUSE_BORDER_RIGHT)
        SDL_WarpMouseInWindow(game->window, HALF_WIDTH, HALF_HEIGHT);

    int rel_x, rel_y;
    SDL_GetRelativeMouseState(&rel_x, &rel_y);
    p->rel = rel_x;
    if (p->rel < -MOUSE_MAX_REL) p->rel = -MOUSE_MAX_REL;
    if (p->rel >  MOUSE_MAX_REL) p->rel =  MOUSE_MAX_REL;

    p->angle += (float)(p->rel * MOUSE_SENSITIVITY * game->delta_time);
}

void player_recover_health(Player *p) {
    Uint32 now = SDL_GetTicks();
    if ((int)(now - p->time_prev) > p->health_recovery_delay) {
        p->time_prev = now;
        if (p->health < PLAYER_MAX_HEALTH)
            p->health += 1;
    }
}

void player_check_game_over(Player *p, Game *game) {
    if (p->health < 1) {
        object_renderer_game_over(&game->object_renderer);
        SDL_RenderPresent(game->renderer);
        SDL_Delay(1500);
        game_new_game(game);
    }
}

void player_get_damage(Player *p, Game *game, int damage) {
    p->health -= damage;
    object_renderer_player_damage(&game->object_renderer);
    if (game->sound.player_pain)
        Mix_PlayChannel(-1, game->sound.player_pain, 0);
    player_check_game_over(p, game);
}

void player_single_fire_event(Player *p, Game *game, const SDL_Event *event) {
    if (event->type == SDL_MOUSEBUTTONDOWN &&
        event->button.button == SDL_BUTTON_LEFT &&
        !p->shot && !game->weapon.reloading)
    {
        if (game->sound.shotgun)
            Mix_PlayChannel(-1, game->sound.shotgun, 0);
        p->shot              = 1;
        game->weapon.reloading = 1;
    }
}

void player_update(Player *p, Game *game) {
    player_movement(p, game);
    player_mouse_control(p, game);
    player_recover_health(p);

}
