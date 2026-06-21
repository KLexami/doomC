#ifndef WEAPON_H
#define WEAPON_H

#include "sprite_object.h"
#include "types.h"
#include "settings.h"

struct Weapon {
    AnimatedSprite base;
    int  weapon_pos_x, weapon_pos_y;
    int  weapon_w, weapon_h;   /* scaled render dimensions */
    int  reloading;
    int  num_images;
    int  frame_counter;
    int  damage;
};

int  weapon_init(Weapon *w, Game *game);
void weapon_update(Weapon *w, Game *game);
void weapon_animate_shot(Weapon *w, Game *game);
void weapon_draw(Weapon *w, SDL_Renderer *renderer);
void weapon_cleanup(Weapon *w);

#endif /* WEAPON_H */
