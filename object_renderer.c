#include "main.h"
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Load PNG, scale to (w, h) if both > 0, return SDL_Texture or NULL */
SDL_Texture *load_texture(SDL_Renderer *renderer, const char *path, int w, int h) {
    SDL_Surface *surf = IMG_Load(path);
    if (!surf) {
        fprintf(stderr, "IMG_Load failed: %s  (%s)\n", path, IMG_GetError());
        return NULL;
    }

    /* Convert to a consistent RGBA format before scaling */
    SDL_Surface *rgba = SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(surf);
    if (!rgba) return NULL;

    SDL_Texture *tex = NULL;
    if (w > 0 && h > 0 && (rgba->w != w || rgba->h != h)) {
        SDL_Surface *scaled = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32,
                                                             SDL_PIXELFORMAT_RGBA32);
        if (scaled) {
            SDL_BlitScaled(rgba, NULL, scaled, NULL);
            tex = SDL_CreateTextureFromSurface(renderer, scaled);
            SDL_FreeSurface(scaled);
        }
        SDL_FreeSurface(rgba);
    } else {
        tex = SDL_CreateTextureFromSurface(renderer, rgba);
        SDL_FreeSurface(rgba);
    }
    return tex;
}

static SDL_Texture *load_wall_tex(SDL_Renderer *renderer, int id) {
    char path[64];
    snprintf(path, sizeof(path), "resources/textures/%d.png", id);
    return load_texture(renderer, path, TEXTURE_SIZE, TEXTURE_SIZE);
}

int object_renderer_init(ObjectRenderer *or_, Game *game) {
    SDL_Renderer *r = game->renderer;
    or_->renderer   = r;
    or_->sky_offset = 0.0f;
    or_->digit_size = 90;

    /* Wall textures 1..5 */
    for (int i = 1; i <= 5; i++)
        or_->wall_textures[i] = load_wall_tex(r, i);

    or_->sky_image = load_texture(r, "resources/textures/sky.png", WIDTH, HALF_HEIGHT);
    or_->blood_screen   = load_texture(r, "resources/textures/blood_screen.png", WIDTH, HEIGHT);
    or_->game_over_image= load_texture(r, "resources/textures/game_over.png",   WIDTH, HEIGHT);
    or_->win_image      = load_texture(r, "resources/textures/win.png",          WIDTH, HEIGHT);

    for (int i = 0; i < 11; i++) {
        char path[64];
        snprintf(path, sizeof(path), "resources/textures/digits/%d.png", i);
        or_->digit_images[i] = load_texture(r, path, or_->digit_size, or_->digit_size);
    }
    return 1;
}

void object_renderer_win(ObjectRenderer *or_) {
    if (or_->win_image) {
        SDL_Rect dst = {0, 0, WIDTH, HEIGHT};
        SDL_RenderCopy(or_->renderer, or_->win_image, NULL, &dst);
    }
}

void object_renderer_game_over(ObjectRenderer *or_) {
    if (or_->game_over_image) {
        SDL_Rect dst = {0, 0, WIDTH, HEIGHT};
        SDL_RenderCopy(or_->renderer, or_->game_over_image, NULL, &dst);
    }
}

void object_renderer_player_damage(ObjectRenderer *or_) {
    if (or_->blood_screen) {
        SDL_Rect dst = {0, 0, WIDTH, HEIGHT};
        SDL_RenderCopy(or_->renderer, or_->blood_screen, NULL, &dst);
    }
}

void object_renderer_draw_player_health(ObjectRenderer *or_, Game *game) {
    char buf[8];
    snprintf(buf, sizeof(buf), "%d", game->player.health);
    int i;
    for (i = 0; buf[i]; i++) {
        int d = buf[i] - '0';
        if (or_->digit_images[d]) {
            SDL_Rect dst = {i * or_->digit_size, 0, or_->digit_size, or_->digit_size};
            SDL_RenderCopy(or_->renderer, or_->digit_images[d], NULL, &dst);
        }
    }
    /* digit 10 = hp icon */
    if (or_->digit_images[10]) {
        SDL_Rect dst = {i * or_->digit_size, 0, or_->digit_size, or_->digit_size};
        SDL_RenderCopy(or_->renderer, or_->digit_images[10], NULL, &dst);
    }
}

void object_renderer_draw_background(ObjectRenderer *or_, Game *game) {
    or_->sky_offset = (float)fmod(
        or_->sky_offset + 4.5f * (float)game->player.rel, (double)WIDTH);

    /* Sky (drawn twice for seamless scroll) */
    if (or_->sky_image) {
        int sx = -(int)or_->sky_offset;
        SDL_Rect d1 = {sx,         0, WIDTH, HALF_HEIGHT};
        SDL_Rect d2 = {sx + WIDTH, 0, WIDTH, HALF_HEIGHT};
        SDL_RenderCopy(or_->renderer, or_->sky_image, NULL, &d1);
        SDL_RenderCopy(or_->renderer, or_->sky_image, NULL, &d2);
    }

    /* Floor */
    SDL_SetRenderDrawColor(or_->renderer, FLOOR_COLOR_R, FLOOR_COLOR_G, FLOOR_COLOR_B, 255);
    SDL_Rect floor = {0, HALF_HEIGHT, WIDTH, HALF_HEIGHT};
    SDL_RenderFillRect(or_->renderer, &floor);
}

/* qsort comparator: sort by depth descending (farthest first) */
static int cmp_objects_desc(const void *a, const void *b) {
    const ObjectToRender *oa = (const ObjectToRender *)a;
    const ObjectToRender *ob = (const ObjectToRender *)b;
    if (oa->depth > ob->depth) return -1;
    if (oa->depth < ob->depth) return  1;
    return 0;
}

void object_renderer_render_game_objects(ObjectRenderer *or_, Game *game) {
    RayCasting *rc = &game->raycasting;
    qsort(rc->objects_to_render, rc->objects_count,
          sizeof(ObjectToRender), cmp_objects_desc);

    for (int i = 0; i < rc->objects_count; i++) {
        ObjectToRender *o = &rc->objects_to_render[i];
        if (!o->texture) continue;
        SDL_Rect *src = o->use_src_rect ? &o->src_rect : NULL;
        SDL_RenderCopy(or_->renderer, o->texture, src, &o->dst_rect);
    }
}

void object_renderer_draw(ObjectRenderer *or_, Game *game) {
    object_renderer_draw_background(or_, game);
    object_renderer_render_game_objects(or_, game);
    object_renderer_draw_player_health(or_, game);
}

void object_renderer_cleanup(ObjectRenderer *or_) {
    for (int i = 1; i < NUM_WALL_TEXTURES; i++)
        if (or_->wall_textures[i]) { SDL_DestroyTexture(or_->wall_textures[i]); or_->wall_textures[i] = NULL; }
    for (int i = 0; i < 11; i++)
        if (or_->digit_images[i]) { SDL_DestroyTexture(or_->digit_images[i]); or_->digit_images[i] = NULL; }

    if (or_->sky_image)      SDL_DestroyTexture(or_->sky_image);
    if (or_->blood_screen)   SDL_DestroyTexture(or_->blood_screen);
    if (or_->game_over_image)SDL_DestroyTexture(or_->game_over_image);
    if (or_->win_image)      SDL_DestroyTexture(or_->win_image);

    or_->sky_image = or_->blood_screen = or_->game_over_image = or_->win_image = NULL;
}
