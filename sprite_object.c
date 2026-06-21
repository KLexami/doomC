#include "main.h"
#include <SDL2/SDL_image.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h> /* FindFirstFile / FindNextFile for directory listing */

/* ---- Directory image loading ---------------------------------------- */

static int cmp_filenames(const void *a, const void *b) {
    /* Numeric sort for "0.png", "1.png" etc.; alphabetic fallback */
    int na = atoi((const char *)a);
    int nb = atoi((const char *)b);
    if (na == 0 && nb == 0)
        return strcmp((const char *)a, (const char *)b);
    return na - nb;
}

int load_images_from_dir(const char *dir_path,
                          SDL_Texture **images, int max_images,
                          SDL_Renderer *renderer)
{
    char search[512];
    snprintf(search, sizeof(search), "%s/*.png", dir_path);

    WIN32_FIND_DATAA fd;
    HANDLE h = FindFirstFileA(search, &fd);
    if (h == INVALID_HANDLE_VALUE) return 0;

    /* Collect filenames */
    char names[MAX_ANIMATION_FRAMES][256];
    int count = 0;
    do {
        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && count < max_images) {
            strncpy(names[count], fd.cFileName, 255);
            names[count][255] = '\0';
            count++;
        }
    } while (FindNextFileA(h, &fd) && count < max_images);
    FindClose(h);

    /* Sort numerically */
    qsort(names, count, sizeof(names[0]), cmp_filenames);

    /* Load each */
    for (int i = 0; i < count; i++) {
        char full[512];
        snprintf(full, sizeof(full), "%s/%s", dir_path, names[i]);
        SDL_Surface *surf = IMG_Load(full);
        if (!surf) {
            fprintf(stderr, "IMG_Load: %s (%s)\n", full, IMG_GetError());
            images[i] = NULL;
            continue;
        }
        images[i] = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);
    }
    return count;
}

/* ---- SpriteObject --------------------------------------------------- */

void sprite_object_init(SpriteObject *s, Game *game,
                        const char *path, float x, float y,
                        float scale, float shift)
{
    s->x = x;
    s->y = y;
    s->sprite_scale        = scale;
    s->sprite_height_shift = shift;
    s->dx = s->dy = s->theta = s->screen_x = 0.0f;
    s->dist = s->norm_dist = 1.0f;
    s->sprite_half_width   = 0.0f;

    SDL_Surface *surf = IMG_Load(path);
    if (surf) {
        s->image        = SDL_CreateTextureFromSurface(game->renderer, surf);
        s->image_width  = surf->w;
        s->image_height = surf->h;
        SDL_FreeSurface(surf);
    } else {
        s->image = NULL;
        s->image_width = s->image_height = 64;
    }
    s->image_half_width = s->image_width / 2;
    s->image_ratio      = (s->image_height > 0)
                          ? (float)s->image_width / (float)s->image_height
                          : 1.0f;
}

void sprite_object_get_sprite_projection(SpriteObject *s, Game *game) {
    double proj       = SCREEN_DIST / (double)s->norm_dist * s->sprite_scale;
    double proj_width = proj * s->image_ratio;
    double proj_height= proj;

    s->sprite_half_width = (float)(proj_width / 2.0);
    float height_shift   = (float)(proj_height * s->sprite_height_shift);

    SDL_Rect dst;
    dst.x = (int)(s->screen_x - s->sprite_half_width);
    dst.y = (int)(HALF_HEIGHT - proj_height / 2.0 + height_shift);
    dst.w = (int)proj_width;
    dst.h = (int)proj_height;

    RayCasting *rc = &game->raycasting;
    if (rc->objects_count < MAX_OBJECTS_TO_RENDER && s->image) {
        ObjectToRender *o = &rc->objects_to_render[rc->objects_count++];
        o->depth        = s->norm_dist;
        o->texture      = s->image;
        o->src_rect     = (SDL_Rect){0, 0, s->image_width, s->image_height};
        o->dst_rect     = dst;
        o->use_src_rect = 0;
    }
}

void sprite_object_get_sprite(SpriteObject *s, Game *game) {
    Player *p = &game->player;

    double dx = s->x - p->x;
    double dy = s->y - p->y;
    s->dx    = (float)dx;
    s->dy    = (float)dy;
    s->theta = (float)atan2(dy, dx);

    double delta = s->theta - p->angle;
    if ((dx > 0 && p->angle > M_PI) || (dx < 0 && dy < 0))
        delta += 2.0 * M_PI;

    double delta_rays = delta / DELTA_ANGLE;
    s->screen_x = (float)((HALF_NUM_RAYS + delta_rays) * SCALE);

    s->dist      = (float)hypot(dx, dy);
    s->norm_dist = s->dist * (float)cos(delta);

    if ((float)(-s->image_half_width) < s->screen_x
        && s->screen_x < (float)(WIDTH + s->image_half_width)
        && s->norm_dist > 0.5f)
    {
        sprite_object_get_sprite_projection(s, game);
    }
}

void sprite_object_update(SpriteObject *s, Game *game) {
    sprite_object_get_sprite(s, game);
}

/* ---- AnimatedSprite ------------------------------------------------- */

int animated_sprite_init(AnimatedSprite *as, Game *game,
                          const char *path, float x, float y,
                          float scale, float shift, int animation_time)
{
    sprite_object_init(&as->base, game, path, x, y, scale, shift);

    as->animation_time      = animation_time;
    as->animation_time_prev = SDL_GetTicks();
    as->animation_trigger   = 0;
    as->current_frame       = 0;

    /* Strip filename → directory */
    strncpy(as->dir_path, path, sizeof(as->dir_path) - 1);
    as->dir_path[sizeof(as->dir_path) - 1] = '\0';
    char *slash = strrchr(as->dir_path, '/');
    if (slash) *slash = '\0';

    as->image_count = load_images_from_dir(as->dir_path, as->images,
                                           MAX_ANIMATION_FRAMES,
                                           game->renderer);
    if (as->image_count > 0 && as->images[0])
        as->base.image = as->images[0];

    return 1;
}

void animated_sprite_check_time(AnimatedSprite *as) {
    as->animation_trigger = 0;
    Uint32 now = SDL_GetTicks();
    if ((int)(now - as->animation_time_prev) > as->animation_time) {
        as->animation_time_prev = now;
        as->animation_trigger   = 1;
    }
}

void animated_sprite_animate(AnimatedSprite *as,
                              SDL_Texture **images, int *frame, int count)
{
    if (as->animation_trigger && count > 0) {
        *frame = (*frame + 1) % count;
        as->base.image = images[*frame];
    }
}

void animated_sprite_update(AnimatedSprite *as, Game *game) {
    sprite_object_get_sprite(&as->base, game);
    animated_sprite_check_time(as);
    animated_sprite_animate(as, as->images, &as->current_frame, as->image_count);
}

void animated_sprite_cleanup(AnimatedSprite *as) {
    for (int i = 0; i < as->image_count; i++)
        if (as->images[i]) { SDL_DestroyTexture(as->images[i]); as->images[i] = NULL; }
    /* base.image points into images[], already freed */
    as->base.image = NULL;
}
