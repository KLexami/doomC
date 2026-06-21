#ifndef SOUND_H
#define SOUND_H

#include "types.h"
#include <SDL2/SDL_mixer.h>

struct Sound {
    Mix_Chunk *shotgun;
    Mix_Chunk *npc_pain;
    Mix_Chunk *npc_death;
    Mix_Chunk *npc_shot;
    Mix_Chunk *player_pain;
    Mix_Music *theme;
};

int  sound_init(Sound *s);
void sound_cleanup(Sound *s);

#endif /* SOUND_H */
