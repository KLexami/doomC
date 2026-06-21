#ifndef NPC_H
#define NPC_H

#include "sprite_object.h"
#include "types.h"
#include "settings.h"

typedef enum NPCType { NPC_SOLDIER, NPC_CACO_DEMON, NPC_CYBER_DEMON } NPCType;

struct NPC {
    AnimatedSprite base;        /* walk/idle frames live in base.images */
    NPCType        type;

    SDL_Texture   *attack_images[MAX_ANIMATION_FRAMES];
    int            attack_count, attack_frame;

    SDL_Texture   *death_images[MAX_ANIMATION_FRAMES];
    int            death_count, death_frame;

    SDL_Texture   *idle_images[MAX_ANIMATION_FRAMES];
    int            idle_count, idle_frame;

    SDL_Texture   *pain_images[MAX_ANIMATION_FRAMES];
    int            pain_count, pain_frame;

    SDL_Texture   *walk_images[MAX_ANIMATION_FRAMES];
    int            walk_count, walk_frame;

    int    attack_dist;
    float  speed;
    int    size;
    int    health;
    int    attack_damage;
    float  accuracy;
    int    alive;
    int    pain;
    int    ray_cast_value;
    int    frame_counter;
    int    player_search_trigger;
};

/* Generic init + type-specific inits */
int  npc_init(NPC *npc, Game *game, NPCType type,
              const char *path, float x, float y,
              float scale, float shift, int animation_time);
int  npc_init_soldier   (NPC *npc, Game *game, float x, float y);
int  npc_init_caco_demon(NPC *npc, Game *game, float x, float y);
int  npc_init_cyber_demon(NPC *npc, Game *game, float x, float y);

void npc_update(NPC *npc, Game *game);
void npc_run_logic(NPC *npc, Game *game);
void npc_movement(NPC *npc, Game *game);
void npc_attack(NPC *npc, Game *game);
void npc_animate_death(NPC *npc, Game *game);
void npc_animate_pain(NPC *npc, Game *game);
void npc_check_hit(NPC *npc, Game *game);
void npc_check_health(NPC *npc, Game *game);
int  npc_ray_cast_player(NPC *npc, Game *game);
int  npc_check_wall(const Game *game, int x, int y);
void npc_check_wall_collision(NPC *npc, const Game *game, float dx, float dy);
void npc_cleanup(NPC *npc);

static inline int npc_map_x(const NPC *npc) { return (int)npc->base.base.x; }
static inline int npc_map_y(const NPC *npc) { return (int)npc->base.base.y; }

#endif /* NPC_H */
