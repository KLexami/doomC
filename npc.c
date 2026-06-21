#include "main.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---- Internal helpers ----------------------------------------------- */

static void load_anim_subdir(NPC *npc, Game *game,
                              const char *subdir,
                              SDL_Texture **images, int *count)
{
    char path[512];
    snprintf(path, sizeof(path), "%s/%s", npc->base.dir_path, subdir);
    *count = load_images_from_dir(path, images, MAX_ANIMATION_FRAMES,
                                  game->renderer);
}

/* ---- Generic init --------------------------------------------------- */

int npc_init(NPC *npc, Game *game, NPCType type,
             const char *path, float x, float y,
             float scale, float shift, int animation_time)
{
    memset(npc, 0, sizeof(*npc));
    animated_sprite_init(&npc->base, game, path, x, y, scale, shift, animation_time);
    npc->type = type;

    load_anim_subdir(npc, game, "attack", npc->attack_images, &npc->attack_count);
    load_anim_subdir(npc, game, "death",  npc->death_images,  &npc->death_count);
    load_anim_subdir(npc, game, "idle",   npc->idle_images,   &npc->idle_count);
    load_anim_subdir(npc, game, "pain",   npc->pain_images,   &npc->pain_count);
    load_anim_subdir(npc, game, "walk",   npc->walk_images,   &npc->walk_count);

    npc->attack_dist          = 3 + rand() % 4; /* randint(3, 6) */
    npc->speed                = 0.03f;
    npc->size                 = 20;
    npc->health               = 100;
    npc->attack_damage        = 10;
    npc->accuracy             = 0.15f;
    npc->alive                = 1;
    npc->pain                 = 0;
    npc->ray_cast_value       = 0;
    npc->frame_counter        = 0;
    npc->player_search_trigger= 0;

    npc->attack_frame = npc->death_frame = 0;
    npc->idle_frame   = npc->pain_frame  = npc->walk_frame = 0;
    return 1;
}

int npc_init_soldier(NPC *npc, Game *game, float x, float y) {
    return npc_init(npc, game, NPC_SOLDIER,
                    "resources/sprites/npc/soldier/0.png",
                    x, y, 0.6f, 0.38f, 180);
}

int npc_init_caco_demon(NPC *npc, Game *game, float x, float y) {
    int r = npc_init(npc, game, NPC_CACO_DEMON,
                     "resources/sprites/npc/caco_demon/0.png",
                     x, y, 0.7f, 0.27f, 250);
    npc->attack_dist   = 1;
    npc->health        = 150;
    npc->attack_damage = 25;
    npc->speed         = 0.05f;
    npc->accuracy      = 0.35f;
    return r;
}

int npc_init_cyber_demon(NPC *npc, Game *game, float x, float y) {
    int r = npc_init(npc, game, NPC_CYBER_DEMON,
                     "resources/sprites/npc/cyber_demon/0.png",
                     x, y, 1.0f, 0.04f, 210);
    npc->attack_dist   = 6;
    npc->health        = 350;
    npc->attack_damage = 15;
    npc->speed         = 0.055f;
    npc->accuracy      = 0.25f;
    return r;
}

/* ---- Wall collision ------------------------------------------------- */

int npc_check_wall(const Game *game, int x, int y) {
    return !map_is_wall(&game->map, x, y);
}

void npc_check_wall_collision(NPC *npc, const Game *game, float dx, float dy) {
    float bx = npc->base.base.x;
    float by = npc->base.base.y;
    float sz = (float)npc->size;
    if (npc_check_wall(game, (int)(bx + dx * sz), (int)by))
        npc->base.base.x += dx;
    if (npc_check_wall(game, (int)bx, (int)(by + dy * sz)))
        npc->base.base.y += dy;
}

/* ---- Movement ------------------------------------------------------- */

void npc_movement(NPC *npc, Game *game) {
    int sx = npc_map_x(npc), sy = npc_map_y(npc);
    int gx = player_map_x(&game->player), gy = player_map_y(&game->player);

    Pos2D next = pathfinding_get_path(&game->pathfinding, game, sx, sy, gx, gy);

    /* Only move if destination cell is not occupied by another NPC */
    if (!game->object_handler.npc_positions[next.y][next.x]) {
        double angle = atan2(next.y + 0.5 - npc->base.base.y,
                             next.x + 0.5 - npc->base.base.x);
        float dx = (float)(cos(angle) * npc->speed);
        float dy = (float)(sin(angle) * npc->speed);
        npc_check_wall_collision(npc, game, dx, dy);
    }
}

/* ---- Attack --------------------------------------------------------- */

void npc_attack(NPC *npc, Game *game) {
    if (npc->base.animation_trigger) {
        if (game->sound.npc_shot)
            Mix_PlayChannel(-1, game->sound.npc_shot, 0);
        float r = (float)rand() / (float)RAND_MAX;
        if (r < npc->accuracy)
            player_get_damage(&game->player, game, npc->attack_damage);
    }
}

/* ---- Animations ----------------------------------------------------- */

void npc_animate_death(NPC *npc, Game *game) {
    if (!npc->alive) {
        if (game->global_trigger && npc->frame_counter < npc->death_count - 1) {
            npc->death_frame = (npc->death_frame + 1) % npc->death_count;
            npc->base.base.image = npc->death_images[npc->death_frame];
            npc->frame_counter++;
        }
    }
}

void npc_animate_pain(NPC *npc, Game *game) {
    animated_sprite_animate(&npc->base,
                            npc->pain_images, &npc->pain_frame, npc->pain_count);
    if (npc->base.animation_trigger)
        npc->pain = 0;
    (void)game;
}

/* ---- Hit detection -------------------------------------------------- */

void npc_check_hit(NPC *npc, Game *game) {
    if (npc->ray_cast_value && game->player.shot) {
        float hw = npc->base.base.sprite_half_width;
        float sx = npc->base.base.screen_x;
        if ((float)(HALF_WIDTH) - hw < sx && sx < (float)(HALF_WIDTH) + hw) {
            if (game->sound.npc_pain)
                Mix_PlayChannel(-1, game->sound.npc_pain, 0);
            game->player.shot = 0;
            npc->pain          = 1;
            npc->health       -= game->weapon.damage;
            npc_check_health(npc, game);
        }
    }
}

void npc_check_health(NPC *npc, Game *game) {
    if (npc->health < 1) {
        npc->alive = 0;
        if (game->sound.npc_death)
            Mix_PlayChannel(-1, game->sound.npc_death, 0);
    }
}

/* ---- Raycasting NPC→Player line-of-sight ---------------------------- */

int npc_ray_cast_player(NPC *npc, Game *game) {
    Player *p   = &game->player;
    Map    *map = &game->map;

    int px = player_map_x(p),  py = player_map_y(p);
    int nx = npc_map_x(npc),   ny = npc_map_y(npc);

    if (px == nx && py == ny) return 1;

    double wall_dist_v = 0, wall_dist_h = 0;
    double player_dist_v = 0, player_dist_h = 0;

    double ox = p->x, oy = p->y;
    double ray = npc->base.base.theta;
    double sin_a = sin(ray), cos_a = cos(ray);

    /* Horizontals */
    double y_hor, dy_h;
    if (sin_a > 0) { y_hor = py + 1;    dy_h =  1.0; }
    else           { y_hor = py - 1e-6;  dy_h = -1.0; }

    double depth_h = (y_hor - oy) / sin_a;
    double x_hor   = ox + depth_h * cos_a;
    double ddh     = dy_h / sin_a;
    double dxh     = ddh * cos_a;

    for (int i = 0; i < MAX_DEPTH; i++) {
        int tx = (int)x_hor, ty = (int)y_hor;
        if (tx == nx && ty == ny) { player_dist_h = depth_h; break; }
        if (map_is_wall(map, tx, ty)) { wall_dist_h = depth_h; break; }
        x_hor += dxh; y_hor += dy_h; depth_h += ddh;
    }

    /* Verticals */
    double x_vert, dx_v;
    if (cos_a > 0) { x_vert = px + 1;    dx_v =  1.0; }
    else           { x_vert = px - 1e-6;  dx_v = -1.0; }

    double depth_v = (x_vert - ox) / cos_a;
    double y_vert  = oy + depth_v * sin_a;
    double ddv     = dx_v / cos_a;
    double dyv     = ddv * sin_a;

    for (int i = 0; i < MAX_DEPTH; i++) {
        int tx = (int)x_vert, ty = (int)y_vert;
        if (tx == nx && ty == ny) { player_dist_v = depth_v; break; }
        if (map_is_wall(map, tx, ty)) { wall_dist_v = depth_v; break; }
        x_vert += dx_v; y_vert += dyv; depth_v += ddv;
    }

    double player_dist = (player_dist_v > player_dist_h) ? player_dist_v : player_dist_h;
    double wall_dist   = (wall_dist_v   > wall_dist_h)   ? wall_dist_v   : wall_dist_h;

    return (player_dist > 0 && player_dist < wall_dist) || wall_dist == 0;
}

/* ---- Main logic ----------------------------------------------------- */

void npc_run_logic(NPC *npc, Game *game) {
    if (npc->alive) {
        npc->ray_cast_value = npc_ray_cast_player(npc, game);
        npc_check_hit(npc, game);

        if (npc->pain) {
            npc_animate_pain(npc, game);
        } else if (npc->ray_cast_value) {
            npc->player_search_trigger = 1;

            if (npc->base.base.dist < (float)npc->attack_dist) {
                animated_sprite_animate(&npc->base,
                    npc->attack_images, &npc->attack_frame, npc->attack_count);
                npc_attack(npc, game);
            } else {
                animated_sprite_animate(&npc->base,
                    npc->walk_images, &npc->walk_frame, npc->walk_count);
                npc_movement(npc, game);
            }
        } else if (npc->player_search_trigger) {
            animated_sprite_animate(&npc->base,
                npc->walk_images, &npc->walk_frame, npc->walk_count);
            npc_movement(npc, game);
        } else {
            animated_sprite_animate(&npc->base,
                npc->idle_images, &npc->idle_frame, npc->idle_count);
        }
    } else {
        npc_animate_death(npc, game);
    }
}

void npc_update(NPC *npc, Game *game) {
    animated_sprite_check_time(&npc->base);
    sprite_object_get_sprite(&npc->base.base, game);
    npc_run_logic(npc, game);
}

/* ---- Cleanup -------------------------------------------------------- */

static void free_tex_array(SDL_Texture **imgs, int count) {
    for (int i = 0; i < count; i++)
        if (imgs[i]) { SDL_DestroyTexture(imgs[i]); imgs[i] = NULL; }
}

void npc_cleanup(NPC *npc) {
    animated_sprite_cleanup(&npc->base);
    free_tex_array(npc->attack_images, npc->attack_count);
    free_tex_array(npc->death_images,  npc->death_count);
    free_tex_array(npc->idle_images,   npc->idle_count);
    free_tex_array(npc->pain_images,   npc->pain_count);
    free_tex_array(npc->walk_images,   npc->walk_count);
}
