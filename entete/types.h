#ifndef TYPES_H
#define TYPES_H

/* Forward declarations for all game structs — breaks circular header deps */
typedef struct Game          Game;
typedef struct Map           Map;
typedef struct Player        Player;
typedef struct RayCasting    RayCasting;
typedef struct ObjectRenderer ObjectRenderer;
typedef struct SpriteObject  SpriteObject;
typedef struct AnimatedSprite AnimatedSprite;
typedef struct NPC           NPC;
typedef struct Weapon        Weapon;
typedef struct Sound         Sound;
typedef struct PathFinding   PathFinding;
typedef struct ObjectHandler ObjectHandler;

/* Shared data-transfer struct: one renderable object (wall slice or sprite) */
#include <SDL2/SDL.h>
typedef struct ObjectToRender {
    float       depth;
    SDL_Texture *texture;
    SDL_Rect     src_rect;
    SDL_Rect     dst_rect;
    int          use_src_rect; /* 1 = wall slice with explicit src_rect */
} ObjectToRender;

/* 2-D integer position (used by pathfinding, map, NPC) */
typedef struct Pos2D { int x, y; } Pos2D;

#endif /* TYPES_H */
