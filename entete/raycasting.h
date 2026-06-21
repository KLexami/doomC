#ifndef RAYCASTING_H
#define RAYCASTING_H

#include "types.h"
#include "settings.h"

/* One ray's intersection result */
typedef struct RaycastHit {
    float depth;
    float proj_height;
    int   texture_id;
    float offset;
} RaycastHit;

struct RayCasting {
    RaycastHit    ray_casting_result[NUM_RAYS];
    ObjectToRender objects_to_render[MAX_OBJECTS_TO_RENDER];
    int           objects_count;
};

void raycasting_init(RayCasting *rc);
void raycasting_update(RayCasting *rc, Game *game);
void raycasting_ray_cast(RayCasting *rc, Game *game);
void raycasting_get_objects_to_render(RayCasting *rc, Game *game);

#endif /* RAYCASTING_H */
