#define SDL_MAIN_HANDLED
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GLOBAL_EVENT_CODE 1

static Uint32 global_timer_callback(Uint32 interval, void *param) {
    SDL_Event ev;
    SDL_memset(&ev, 0, sizeof(ev));
    ev.type       = SDL_USEREVENT;
    ev.user.code  = GLOBAL_EVENT_CODE;
    SDL_PushEvent(&ev);
    (void)param;
    return interval;
}

/* ---- Game init / new_game ------------------------------------------ */

int game_init(Game *game) {
    memset(game, 0, sizeof(*game));
    SDL_SetMainReady();

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return 0;
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        fprintf(stderr, "IMG_Init: %s\n", IMG_GetError());
        return 0;
    }

    game->window = SDL_CreateWindow(
        "DOOM-C",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT,
        SDL_WINDOW_SHOWN
    );
    if (!game->window) {
        fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
        return 0;
    }

    game->renderer = SDL_CreateRenderer(game->window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!game->renderer) {
        fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError());
        return 0;
    }

    SDL_ShowCursor(SDL_DISABLE);
    SDL_SetWindowGrab(game->window, SDL_TRUE);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    SDL_AddTimer(40, global_timer_callback, NULL);

    game->running    = 1;
    game->delta_time = 1.0f;

    game_new_game(game);
    return 1;
}

void game_new_game(Game *game) {
    object_handler_cleanup(&game->object_handler);
    object_renderer_cleanup(&game->object_renderer);
    weapon_cleanup(&game->weapon);
    sound_cleanup(&game->sound);

    map_init(&game->map);
    player_init(&game->player);
    object_renderer_init(&game->object_renderer, game);
    raycasting_init(&game->raycasting);
    object_handler_init(&game->object_handler, game);
    weapon_init(&game->weapon, game);
    sound_init(&game->sound);
    pathfinding_init(&game->pathfinding, game);

    if (game->sound.theme)
        Mix_PlayMusic(game->sound.theme, -1);
}

/* ---- Game loop ------------------------------------------------------ */

void game_check_events(Game *game) {
    game->global_trigger = 0;
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT) {
            game->running = 0;
            return;
        }
        if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE) {
            game->running = 0;
            return;
        }
        if (ev.type == SDL_USEREVENT && ev.user.code == GLOBAL_EVENT_CODE)
            game->global_trigger = 1;

        player_single_fire_event(&game->player, game, &ev);
    }
}

void game_update(Game *game) {
    static Uint32 last_ticks = 0;
    Uint32 now = SDL_GetTicks();
    if (last_ticks == 0) last_ticks = now;
    float dt = (float)(now - last_ticks);
    if (dt < 1.0f) dt = 1.0f;
    game->delta_time = dt;
    last_ticks = now;

    player_update(&game->player, game);
    raycasting_update(&game->raycasting, game);
    object_handler_update(&game->object_handler, game);
    weapon_update(&game->weapon, game);

    SDL_RenderPresent(game->renderer);

    /* FPS in window title */
    if (dt > 0.0f) {
        char title[32];
        snprintf(title, sizeof(title), "DOOM-C  %.1f fps", 1000.0f / dt);
        SDL_SetWindowTitle(game->window, title);
    }
}

void game_draw(Game *game) {
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
    SDL_RenderClear(game->renderer);
    object_renderer_draw(&game->object_renderer, game);
    weapon_draw(&game->weapon, game->renderer);
}

void game_run(Game *game) {
    while (game->running) {
        game_check_events(game);
        game_draw(game);
        game_update(game);
    }
}

void game_cleanup(Game *game) {
    object_handler_cleanup(&game->object_handler);
    object_renderer_cleanup(&game->object_renderer);
    weapon_cleanup(&game->weapon);
    sound_cleanup(&game->sound);
    if (game->renderer) SDL_DestroyRenderer(game->renderer);
    if (game->window)   SDL_DestroyWindow(game->window);
    IMG_Quit();
    SDL_Quit();
}

/* ---- Entry point ---------------------------------------------------- */

int main(int argc, char *argv[]) {
    (void)argc; (void)argv;

    Game game;
    if (!game_init(&game)) return 1;
    game_run(&game);
    game_cleanup(&game);
    return 0;
}
