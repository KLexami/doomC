#ifndef SPRITE_OBJECT_H
#define SPRITE_OBJECT_H

#include "types.h"
#include "settings.h"
#include <SDL2/SDL.h>

/* ---- SpriteObject (base) -------------------------------------------- */
struct SpriteObject {
    float        x, y;
    SDL_Texture *image;          /* current frame texture */
    int          image_width;
    int          image_height;
    int          image_half_width;
    float        image_ratio;    /* width / height */
    float        dx, dy;
    float        theta;
    float        screen_x;
    float        dist;
    float        norm_dist;
    float        sprite_half_width;
    float        sprite_scale;
    float        sprite_height_shift;
};

/* ---- AnimatedSprite (extends SpriteObject) --------------------------- */
struct AnimatedSprite {
    SpriteObject  base;
    int           animation_time;    /* ms per frame */
    char          dir_path[512];     /* directory containing frames */
    SDL_Texture  *images[MAX_ANIMATION_FRAMES];
    int           image_count;
    int           current_frame;
    Uint32        animation_time_prev;
    int           animation_trigger;
};

/* SpriteObject functions */
void sprite_object_init(SpriteObject *s, Game *game,
                        const char *path, float x, float y,
                        float scale, float shift);
void sprite_object_get_sprite(SpriteObject *s, Game *game);
void sprite_object_get_sprite_projection(SpriteObject *s, Game *game);
void sprite_object_update(SpriteObject *s, Game *game);

/* AnimatedSprite functions */
int  animated_sprite_init(AnimatedSprite *as, Game *game,
                          const char *path, float x, float y,
                          float scale, float shift, int animation_time);
void animated_sprite_update(AnimatedSprite *as, Game *game);
void animated_sprite_check_time(AnimatedSprite *as);
void animated_sprite_animate(AnimatedSprite *as,
                              SDL_Texture **images, int *frame, int count);
void animated_sprite_cleanup(AnimatedSprite *as);

/* Utility: load all PNGs from a directory into an array; returns count */
int load_images_from_dir(const char *dir_path,
                         SDL_Texture **images, int max_images,
                         SDL_Renderer *renderer);

#endif /* SPRITE_OBJECT_H */
