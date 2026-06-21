#ifndef OBJECT_HANDLER_H
#define OBJECT_HANDLER_H

#include "types.h"
#include "settings.h"
#include "sprite_object.h"
#include "npc.h"

struct ObjectHandler {
    AnimatedSprite *sprites[MAX_SPRITES];
    int             sprite_count;

    NPC            *npcs[MAX_NPCS];
    int             npc_count;

    /* 2-D boolean grid: 1 = an alive NPC occupies this cell */
    int             npc_positions[MAP_ROWS][MAP_COLS];

    int             enemy_count;
};

int  object_handler_init(ObjectHandler *oh, Game *game);
void object_handler_update(ObjectHandler *oh, Game *game);
void object_handler_check_win(ObjectHandler *oh, Game *game);
void object_handler_spawn_npcs(ObjectHandler *oh, Game *game);
void object_handler_add_sprite(ObjectHandler *oh, AnimatedSprite *sprite);
void object_handler_add_npc(ObjectHandler *oh, NPC *npc);
void object_handler_cleanup(ObjectHandler *oh);

#endif /* OBJECT_HANDLER_H */
