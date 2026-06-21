#include "main.h"
#include <SDL2/SDL_image.h>
#include <string.h>
#include <stdio.h>

int weapon_init(Weapon *w, Game *game) {
    memset(w, 0, sizeof(*w));

    const char *path = "resources/sprites/weapon/shotgun/0.png";
    float scale = 0.4f;
    int   anim_time = 90;

    /* Init the base AnimatedSprite (pos doesn't matter for a HUD weapon) */
    animated_sprite_init(&w->base, game, path, 0.0f, 0.0f, scale, 0.0f, anim_time);

    w->reloading    = 0;
    w->num_images   = w->base.image_count;
    w->frame_counter= 0;
    w->damage       = 50;

    /* Compute scaled render dimensions from the first frame */
    int orig_w = 0, orig_h = 0;
    if (w->base.base.image)
        SDL_QueryTexture(w->base.base.image, NULL, NULL, &orig_w, &orig_h);

    w->weapon_w = (int)(orig_w * scale);
    w->weapon_h = (int)(orig_h * scale);
    w->weapon_pos_x = HALF_WIDTH - w->weapon_w / 2;
    w->weapon_pos_y = HEIGHT - w->weapon_h;

    return 1;
}

void weapon_animate_shot(Weapon *w, Game *game) {
    if (w->reloading) {
        game->player.shot = 0;
        if (w->base.animation_trigger) {
            w->base.current_frame = (w->base.current_frame + 1) % w->base.image_count;
            w->base.base.image    = w->base.images[w->base.current_frame];
            w->frame_counter++;
            if (w->frame_counter == w->num_images) {
                w->reloading     = 0;
                w->frame_counter = 0;
                /* reset to first frame */
                w->base.current_frame = 0;
                w->base.base.image    = w->base.images[0];
            }
        }
    }
}

void weapon_update(Weapon *w, Game *game) {
    animated_sprite_check_time(&w->base);
    weapon_animate_shot(w, game);
}

void weapon_draw(Weapon *w, SDL_Renderer *renderer) {
    SDL_Texture *tex = w->base.images[w->base.current_frame];
    if (!tex) return;
    SDL_Rect dst = { w->weapon_pos_x, w->weapon_pos_y, w->weapon_w, w->weapon_h };
    SDL_RenderCopy(renderer, tex, NULL, &dst);
}

void weapon_cleanup(Weapon *w) {
    animated_sprite_cleanup(&w->base);
}
