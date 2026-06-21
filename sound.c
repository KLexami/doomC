#include "sound.h"
#include <stdio.h>

int sound_init(Sound *s) {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "SDL_mixer init error: %s\n", Mix_GetError());
        return 0;
    }

    const char *base = "resources/sound/";

    s->shotgun    = Mix_LoadWAV("resources/sound/shotgun.wav");
    s->npc_pain   = Mix_LoadWAV("resources/sound/npc_pain.wav");
    s->npc_death  = Mix_LoadWAV("resources/sound/npc_death.wav");
    s->npc_shot   = Mix_LoadWAV("resources/sound/npc_attack.wav");
    s->player_pain= Mix_LoadWAV("resources/sound/player_pain.wav");
    s->theme      = Mix_LoadMUS("resources/sound/theme.mp3");

    if (s->npc_shot)
        Mix_VolumeChunk(s->npc_shot, (int)(MIX_MAX_VOLUME * 0.2f));

    Mix_VolumeMusic((int)(MIX_MAX_VOLUME * 0.1f));

    (void)base;
    return 1;
}

void sound_cleanup(Sound *s) {
    if (s->shotgun)    { Mix_FreeChunk(s->shotgun);    s->shotgun    = NULL; }
    if (s->npc_pain)   { Mix_FreeChunk(s->npc_pain);   s->npc_pain   = NULL; }
    if (s->npc_death)  { Mix_FreeChunk(s->npc_death);  s->npc_death  = NULL; }
    if (s->npc_shot)   { Mix_FreeChunk(s->npc_shot);   s->npc_shot   = NULL; }
    if (s->player_pain){ Mix_FreeChunk(s->player_pain);s->player_pain= NULL; }
    if (s->theme)      { Mix_FreeMusic(s->theme);      s->theme      = NULL; }
    Mix_CloseAudio();
}
