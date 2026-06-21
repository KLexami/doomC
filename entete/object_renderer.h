#ifndef OBJECT_RENDERER_H
#define OBJECT_RENDERER_H

#include "types.h"
#include "settings.h"
#include <SDL2/SDL.h>

/* digit 10 is the heart/hp icon */
#define NUM_WALL_TEXTURES  6   /* indices 1..5 */
#define NUM_DIGIT_TEXTURES 11  /* 0..10 */

struct ObjectRenderer {
    SDL_Renderer *renderer;
    SDL_Texture  *wall_textures[NUM_WALL_TEXTURES]; /* [1]..[5], [0] unused */
    SDL_Texture  *sky_image;
    float         sky_offset;
    SDL_Texture  *blood_screen;
    SDL_Texture  *game_over_image;
    SDL_Texture  *win_image;
    SDL_Texture  *digit_images[NUM_DIGIT_TEXTURES];
    int           digit_size;
};

int  object_renderer_init(ObjectRenderer *or_, Game *game);
void object_renderer_draw(ObjectRenderer *or_, Game *game);
void object_renderer_draw_background(ObjectRenderer *or_, Game *game);
void object_renderer_render_game_objects(ObjectRenderer *or_, Game *game);
void object_renderer_draw_player_health(ObjectRenderer *or_, Game *game);
void object_renderer_player_damage(ObjectRenderer *or_);
void object_renderer_win(ObjectRenderer *or_);
void object_renderer_game_over(ObjectRenderer *or_);
void object_renderer_cleanup(ObjectRenderer *or_);

/* Utility: load PNG and scale to (w,h) */
SDL_Texture *load_texture(SDL_Renderer *renderer, const char *path, int w, int h);

#endif /* OBJECT_RENDERER_H */
