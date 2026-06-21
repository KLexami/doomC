#include "main.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

void object_handler_add_sprite(ObjectHandler *oh, AnimatedSprite *sprite) {
    if (oh->sprite_count < MAX_SPRITES)
        oh->sprites[oh->sprite_count++] = sprite;
}

void object_handler_add_npc(ObjectHandler *oh, NPC *npc) {
    if (oh->npc_count < MAX_NPCS)
        oh->npcs[oh->npc_count++] = npc;
}

/* Spawn NPCs at random walkable positions outside the restricted area (0..9,0..9) */
void object_handler_spawn_npcs(ObjectHandler *oh, Game *game) {
    for (int i = 0; i < oh->enemy_count; i++) {
        /* Choose NPC type: weights [70, 20, 10] */
        int r = rand() % 100;
        NPCType type;
        if      (r < 70) type = NPC_SOLDIER;
        else if (r < 90) type = NPC_CACO_DEMON;
        else             type = NPC_CYBER_DEMON;

        /* Random free cell outside restricted area */
        int x, y;
        int tries = 0;
        do {
            x = rand() % game->map.cols;
            y = rand() % game->map.rows;
            tries++;
        } while ((map_is_wall(&game->map, x, y) || (x < 10 && y < 10))
                 && tries < 10000);

        NPC *npc = (NPC *)malloc(sizeof(NPC));
        if (!npc) continue;

        switch (type) {
            case NPC_SOLDIER:     npc_init_soldier(npc,     game, x + 0.5f, y + 0.5f); break;
            case NPC_CACO_DEMON:  npc_init_caco_demon(npc,  game, x + 0.5f, y + 0.5f); break;
            case NPC_CYBER_DEMON: npc_init_cyber_demon(npc, game, x + 0.5f, y + 0.5f); break;
        }
        object_handler_add_npc(oh, npc);
    }
}

static AnimatedSprite *make_sprite(Game *game, const char *path, float x, float y) {
    AnimatedSprite *s = (AnimatedSprite *)malloc(sizeof(AnimatedSprite));
    if (!s) return NULL;
    animated_sprite_init(s, game, path, x, y, 0.8f, 0.16f, 120);
    return s;
}

int object_handler_init(ObjectHandler *oh, Game *game) {
    memset(oh, 0, sizeof(*oh));
    oh->enemy_count = 20;

    srand((unsigned)time(NULL));
    object_handler_spawn_npcs(oh, game);

    const char *green = "resources/sprites/animated_sprites/green_light/0.png";
    const char *red   = "resources/sprites/animated_sprites/red_light/0.png";

    /* Green lights */
    struct { float x, y; } green_pos[] = {
        {11.5f, 3.5f}, {1.5f, 1.5f}, {1.5f, 7.5f},
        {5.5f, 3.25f}, {5.5f, 4.75f}, {7.5f, 2.5f},
        {7.5f, 5.5f},  {14.5f, 1.5f}, {14.5f, 4.5f},
        {14.5f, 24.5f},{14.5f, 30.5f},{1.5f, 30.5f},{1.5f, 24.5f}
    };
    for (int i = 0; i < (int)(sizeof(green_pos)/sizeof(green_pos[0])); i++) {
        AnimatedSprite *s = make_sprite(game, green, green_pos[i].x, green_pos[i].y);
        if (s) object_handler_add_sprite(oh, s);
    }

    /* Red lights */
    struct { float x, y; } red_pos[] = {
        {14.5f, 5.5f}, {14.5f, 7.5f}, {12.5f, 7.5f},
        {9.5f, 7.5f},  {14.5f, 12.5f},{9.5f, 20.5f},
        {10.5f, 20.5f},{3.5f, 14.5f}, {3.5f, 18.5f}
    };
    for (int i = 0; i < (int)(sizeof(red_pos)/sizeof(red_pos[0])); i++) {
        AnimatedSprite *s = make_sprite(game, red, red_pos[i].x, red_pos[i].y);
        if (s) object_handler_add_sprite(oh, s);
    }

    return 1;
}

void object_handler_check_win(ObjectHandler *oh, Game *game) {
    int alive = 0;
    for (int i = 0; i < oh->npc_count; i++)
        if (oh->npcs[i]->alive) alive++;

    if (alive == 0) {
        object_renderer_win(&game->object_renderer);
        SDL_RenderPresent(game->renderer);
        SDL_Delay(1500);
        game_new_game(game);
    }
}

void object_handler_update(ObjectHandler *oh, Game *game) {
    /* Rebuild alive NPC position grid */
    memset(oh->npc_positions, 0, sizeof(oh->npc_positions));
    for (int i = 0; i < oh->npc_count; i++) {
        NPC *npc = oh->npcs[i];
        if (npc->alive) {
            int nx = npc_map_x(npc), ny = npc_map_y(npc);
            if (nx >= 0 && nx < MAP_COLS && ny >= 0 && ny < MAP_ROWS)
                oh->npc_positions[ny][nx] = 1;
        }
    }

    for (int i = 0; i < oh->sprite_count; i++)
        animated_sprite_update(oh->sprites[i], game);

    for (int i = 0; i < oh->npc_count; i++)
        npc_update(oh->npcs[i], game);

    object_handler_check_win(oh, game);
}

void object_handler_cleanup(ObjectHandler *oh) {
    for (int i = 0; i < oh->sprite_count; i++) {
        animated_sprite_cleanup(oh->sprites[i]);
        free(oh->sprites[i]);
        oh->sprites[i] = NULL;
    }
    for (int i = 0; i < oh->npc_count; i++) {
        npc_cleanup(oh->npcs[i]);
        free(oh->npcs[i]);
        oh->npcs[i] = NULL;
    }
    oh->sprite_count = 0;
    oh->npc_count    = 0;
}
